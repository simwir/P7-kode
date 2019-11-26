/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler
 *Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 *associated documentation files (the "Software"), to deal in the Software without restriction,
 *including without limitation the rights to use, copy, modify, merge, publish, distribute,
 *sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or
 *substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 *NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
 *OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "robot.hpp"
#include "geo/geo.hpp"
#include "tcp/server.hpp"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>

constexpr double DIST_TO_GOAL_THRESHOLD = 0.01;
constexpr double ROBOT_RADIUS = 0.021;

geo::RelPoint from_lidar_point(const webots::LidarPoint &point)
{
    return {point.x, point.z};
}

std::ostream &operator<<(std::ostream &os, const webots::LidarPoint &point)
{
    return os << point.x << ", " << point.z;
}

void RobotController::dump_readings_to_csv(const std::string &pcfilename,
                                           const std::string &rangefilename)
{
    std::ofstream pc{pcfilename};
    for (auto pt : point_cloud) {
        pc << pt << std::endl;
    }
    std::ofstream rangefile{rangefilename};
    for (size_t i = 0; i < lidar_range_values.size(); ++i) {
        rangefile << i << ',';
        if (lidar_range_values[i].has_value()) {
            rangefile << lidar_range_values[i].value();
        }
        else {
            rangefile << 1.1 * lidar_max_range;
        }
        rangefile << '\n';
    }
}

int getRobotId(webots::Supervisor *robot)
{
    auto self = robot->getSelf();
    if (self == nullptr) {
        std::cerr << "robot->getSelf() returned 0. Please restart webots";
        exit(1);
    }
    return self->getField("id")->getSFInt32();
}

RobotController::RobotController(webots::Supervisor *robot)
    : time_step((int)robot->getBasicTimeStep()), robot(robot),
      server(std::to_string(getRobotId(robot))),
      dfollowed(std::numeric_limits<double>::max())
{
    left_motor = robot->getMotor("left wheel motor");
    right_motor = robot->getMotor("right wheel motor");

    left_motor->setPosition(INFINITY);
    right_motor->setPosition(INFINITY);
    left_motor->setVelocity(0);
    right_motor->setVelocity(0);

    frontGPS = robot->getGPS("gps1");
    backGPS = robot->getGPS("gps2");
    frontGPS->enable(time_step);
    backGPS->enable(time_step);

    lidar = {robot->getLidar("lidar")};
    lidar.enable(time_step);
    lidar_resolution = lidar.get_resolution();
    lidar_max_range = lidar.get_max_range();
    lidar_min_range = lidar.get_min_range();
    lidar_range_values.resize(lidar_resolution);
    // point_cloud.resize(lidar_resolution);
    lidar_fov = lidar.get_fov();

    for (int i = 0; i < NUM_SENSORS; ++i) {
        distance_sensors[i] = robot->getDistanceSensor("ps" + std::to_string(i));
        distance_sensors[i]->enable(time_step);
    }
    for (int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = robot->getLED("led" + std::to_string(i));
    }
}

void RobotController::update_sensor_values()
{
    std::transform(distance_sensors.begin(), distance_sensors.end(), sensor_readings.begin(),
                   [](auto *ds) { return ds->getValue(); });

    position = get_position();
    cur_dist2goal = geo::euclidean_dist(position, goal);

    const float *range_image = lidar.get_range_image();
    const int quarter = lidar_resolution / 4;
    for (int i = 0; i < lidar_resolution; ++i) {
        const int index =
            lidar_resolution - 1 - (i - quarter + lidar_resolution) % lidar_resolution;

        float dist = range_image[i];
        auto nextval = 1.01 * lidar_min_range <= dist && dist <= 0.99 * lidar_max_range
                           ? std::make_optional(dist)
                           : std::nullopt;

        lidar_range_values[index] = nextval;
    }
}

void RobotController::communicate()
{
    using namespace webots_server;
    auto msg = server.get_message();
    if (!msg)
        return;
    auto message = *msg;
    switch (message.type) {
    case MessageType::get_state: {
        auto response = std::to_string(position.x) + "," + std::to_string(position.y) + "," +
                        (is_stopped ? "holding" : "running");
        server.send_message({response, MessageType::get_state});
        break;
    }
    case MessageType::set_destination: {
        size_t split_pos = message.payload.find(",");
        if (split_pos == std::string::npos) {
            server.send_message({message.payload, MessageType::not_understood});
            return;
        }
        goal = {std::stod(message.payload.substr(0, split_pos)),
                       std::stod(message.payload.substr(split_pos + 1))};
        break;
    }
    default:
        break;
    }
}

void RobotController::run_simulation()
{
    set_goal(geo::GlobalPoint{-0.68, 0.79});
    while (robot->step(time_step) != -1) {
        communicate();
        if (first_iteration) {
            first_iteration = false;
            continue;
        }

        if (!has_goal) {
            stop();
            continue;
        }

        update_sensor_values();

        if (cur_dist2goal < DIST_TO_GOAL_THRESHOLD) {
            stop();
            continue;
        }

        if (phase == Phase::BoundaryFollowing) {
            phase = boundary_following();
        }
        else {
            phase = motion2goal();
        }
    }
}

Phase RobotController::motion2goal()
{
    prev_dist2goal = cur_dist2goal;

    // Straight to goal
    geo::Angle angle2goal = get_relative_angle_to_goal();
    int lidar_value_index = angle2index(angle2goal);
    std::optional<double> range2goal = lidar_range_values[lidar_value_index];
    if ((!range2goal.has_value() || cur_dist2goal < range2goal.value()) && clear()) {
        std::cerr << "Towards goal: " << angle2goal << ", " << !range2goal.has_value() << std::endl;
        go_towards_angle(angle2goal);

        return Phase::Motion2Goal;
    }

    // Around obstacles
    auto discontinuities = get_discontinuity_points();

    double min_heuristic = std::numeric_limits<double>::max();
    geo::GlobalPoint best_point;
    DiscontinuityDirection dir;
    for (const auto &[point, direction] : discontinuities) {
        double h = geo::euclidean_dist(position, point) + geo::euclidean_dist(point, goal);

        if (h < min_heuristic) {
            min_heuristic = h;
            best_point = point;
            dir = direction;
        }
    }

    if (geo::euclidean_dist(best_point, goal) > prev_dist2goal) {
        std::cerr << "Changing phase to boundary following" << std::endl;
        dfollowed = std::numeric_limits<double>::max();
        update_dfollowed();
        return Phase::BoundaryFollowing;
    }
    else {
        go_to_discontinuity(best_point, dir);

        return Phase::Motion2Discontinuity;
    }
}

void RobotController::go_to_discontinuity(geo::GlobalPoint point, DiscontinuityDirection dir)
{
    geo::Angle angle = get_angle_to_point(point);
    std::cerr << "Towards discontinuity: " << angle << ", " << (dir == DiscontinuityDirection::Left)
              << std::endl;

    const double dist = geo::euclidean_dist(position, point);
    const double rectified_remaining_dist = (1 - std::min(1.0, dist));
    const double angle_correction =
        rectified_remaining_dist * PI / 4 * (dir == DiscontinuityDirection::Left ? 1 : -1);

    go_towards_angle(angle + angle_correction);
}

bool RobotController::clear()
{
    if (phase == Phase::Motion2Goal) {
        return true;
    }

    for (int i = 0; i < lidar_resolution; ++i) {
        const geo::Angle angle = index2angle(i);
        const geo::Angle diff = abs_angle(angle - get_relative_angle_to_goal());

        if (diff.theta <= PI / 2 && lidar_range_values[i].has_value() &&
            abs(geo::RelPoint::from_polar(lidar_range_values[i].value(), diff).y) <= ROBOT_RADIUS) {

            return false;
        }
    }

    return true;
}

Phase RobotController::boundary_following()
{
    if (dreach() + 0.05 < dfollowed) {
        std::cerr << "Changing phase to motion to goal" << std::endl;
        return Phase::Motion2Goal;
    }

    auto discontinuities = get_discontinuity_points();

    double best_angle = std::numeric_limits<double>::max();
    geo::GlobalPoint best_point;
    DiscontinuityDirection dir;
    for (const auto &[point, direction] : discontinuities) {
        const geo::Angle point_angle = get_angle_to_point(point);
        if (abs_angle(point_angle).theta < best_angle) {
            best_point = point;
            dir = direction;
            best_angle = abs_angle(point_angle).theta;
        }
    }

    go_to_discontinuity(best_point, dir);

    update_dfollowed();
    return Phase::BoundaryFollowing;
}

void RobotController::update_dfollowed()
{
    double obstacle2goal = std::numeric_limits<double>::max();

    const geo::Angle facing_angle = get_facing_angle();

    for (int i = 0; i < lidar_resolution; ++i) {
        if (lidar_range_values[i].has_value()) {
            const geo::Angle angle = index2angle(i);

            geo::RelPoint point = geo::RelPoint::from_polar(lidar_range_values[i].value(), angle);

            const geo::GlobalPoint global_point =
                geo::to_global_coordinates(position, facing_angle, point);

            obstacle2goal = std::min(obstacle2goal, geo::euclidean_dist(global_point, goal));
        }
    }

    dfollowed = std::min(dfollowed, obstacle2goal);
}

geo::Angle RobotController::normal_angle()
{
    std::vector<geo::Angle> candidates;
    double normal_dist = std::numeric_limits<double>::max();

    for (int i = 0; i < lidar_resolution; ++i) {
        if (!lidar_range_values[i].has_value()) {
            continue;
        }

        if (lidar_range_values[i].value() < (normal_dist - 0.0001)) {
            candidates.clear();
            candidates.push_back(index2angle(i));
            normal_dist = lidar_range_values[i].value();
        }
        else if (abs(lidar_range_values[i].value() - normal_dist) < 0.0001) {
            candidates.push_back(index2angle(i));
            normal_dist =
                0.99 * normal_dist + 0.01 * lidar_range_values[i].value(); // Exponential decay
        }
    }

    double sum = 0;
    for (const geo::Angle angle : candidates) {
        sum += angle.theta;
    }

    return geo::Angle{sum / candidates.size()};
}

double RobotController::dreach()
{
    double min_dist = std::numeric_limits<double>::max();

    const geo::Angle facing_angle = get_facing_angle();

    for (int i = 0; i < lidar_resolution; ++i) {
        const double dist =
            lidar_range_values[i].has_value() ? lidar_range_values[i].value() : lidar_max_range;
        const geo::Angle angle = index2angle(i);

        geo::RelPoint point = geo::RelPoint::from_polar(dist, angle);

        const geo::GlobalPoint global_point =
            geo::to_global_coordinates(position, facing_angle, point);

        min_dist = std::min(min_dist, geo::euclidean_dist(global_point, goal));
    }

    return min_dist;
}

void RobotController::go_towards_angle(const geo::Angle &angle)
{
    geo::Angle abs = geo::abs_angle(angle);

    if (angle.theta > PI / 10) {
        std::cerr << "Turning left" << std::endl;
        do_left_turn();
    }
    else if (angle.theta < -PI / 10) {
        std::cerr << "Turning right" << std::endl;
        do_right_turn();
    }
    else if (abs.theta < 0.5 * (PI / 180)) { // If we are off by less than 0.5 degrees, we will not
                                             // perform any correction
        std::cerr << "Going straight ahead" << std::endl;
        go_straight_ahead();
    }
    else {
        std::cerr << "Going straight with adjustment" << std::endl;
        go_adjusted_straight(angle);
    }
}

void RobotController::go_adjusted_straight(const geo::Angle &angle)
{
    left_motor->setVelocity(8 - 3.5 * angle.theta);
    right_motor->setVelocity(8 + 3.5 * angle.theta);
    set_leds(Direction::Straight);
    is_stopped = false;
}

void RobotController::go_straight_ahead()
{
    left_motor->setVelocity(6);
    right_motor->setVelocity(6);
    set_leds(Direction::Straight);
}

void RobotController::do_left_turn()
{
    left_motor->setVelocity(-4);
    right_motor->setVelocity(4);
    is_stopped = false;
    set_leds(Direction::Left);
}

void RobotController::do_right_turn()
{
    left_motor->setVelocity(4);
    right_motor->setVelocity(-4);
    is_stopped = false;
    set_leds(Direction::Right);
}

void RobotController::set_leds(Direction dir)
{
    leds[0]->set(0);
    leds[1]->set(dir == Direction::Left);
    leds[2]->set(dir == Direction::Left);
    leds[3]->set(dir == Direction::Left);
    leds[4]->set(dir == Direction::Right);
    leds[5]->set(dir == Direction::Right);
    leds[6]->set(dir == Direction::Right);
    leds[7]->set(0);
}

void RobotController::stop()
{
    left_motor->setVelocity(0);
    right_motor->setVelocity(0);
    is_stopped = true;
}

geo::Angle RobotController::get_facing_angle() const
{
    return geo::angle_of_line(gps_reading_to_point(backGPS), gps_reading_to_point(frontGPS));
}

geo::Angle RobotController::get_angle_to_goal() const
{
    return geo::angle_of_line(get_position(), goal);
}

geo::Angle RobotController::get_relative_angle_to_goal() const
{
    return get_angle_to_goal() - get_facing_angle();
}

geo::Angle RobotController::get_angle_to_point(const geo::GlobalPoint &point) const
{
    geo::Angle angle = geo::angle_of_line(get_position(), point);
    return angle - get_facing_angle();
}

geo::GlobalPoint RobotController::get_position() const
{
    return geo::get_midpoint(gps_reading_to_point(frontGPS), gps_reading_to_point(backGPS));
}

std::vector<std::pair<geo::GlobalPoint, DiscontinuityDirection>>
RobotController::get_discontinuity_points() const
{
    std::vector<std::pair<geo::GlobalPoint, DiscontinuityDirection>> discontinuities;
    constexpr float CONTINUITY_THRESHOLD = 0.05f;
    const geo::Angle facing_angle = get_facing_angle();

    for (int i = 0; i < lidar_resolution; ++i) {
        int next = (i + 1) % lidar_resolution;
        // Switch from object in range to not in range or vice versa
        if (lidar_range_values[i].has_value() != lidar_range_values[next].has_value()) {
            const double dist =
                lidar_range_values[i].has_value() ? lidar_range_values[i].value() : lidar_max_range;
            const geo::Angle angle = index2angle(i);

            geo::RelPoint point = geo::RelPoint::from_polar(dist, angle);
            DiscontinuityDirection direction = DiscontinuityDirection::Right;

            if (lidar_range_values[i].has_value()) {
                direction = DiscontinuityDirection::Left;
            }

            discontinuities.push_back(std::make_pair(
                geo::to_global_coordinates(position, facing_angle, point), direction));
        }
        // big jump in object distance - assume new object.
        else if (lidar_range_values[i].has_value() &&
                 abs(lidar_range_values[i].value() - lidar_range_values[next].value()) >
                     CONTINUITY_THRESHOLD) {
            const geo::Angle angle = index2angle(i);

            geo::RelPoint point = geo::RelPoint::from_polar(lidar_range_values[i].value(), angle);
            DiscontinuityDirection direction = DiscontinuityDirection::Right;

            if (lidar_range_values[i].value() < lidar_range_values[next].value()) {
                direction = DiscontinuityDirection::Left;
            }

            discontinuities.push_back(std::make_pair(
                geo::to_global_coordinates(position, facing_angle, point), direction));
        }
    }

    return discontinuities;
}
