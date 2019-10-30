#include "robot.hpp"
#include "geo/geo.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>

constexpr double DIST_TO_GOAL_THRESHOLD = 0.1;

geo::RelPoint from_lidar_point(const webots::LidarPoint &point)
{
    return {point.x, point.z};
}

std::ostream &operator<<(std::ostream &os, const webots::LidarPoint &point)
{
    return os << point.x << ", " << point.z;
}

void robot_controller::dump_readings_to_csv(const std::string &pcfilename,
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

robot_controller::robot_controller(webots::Supervisor *robot)
    : time_step((int)robot->getBasicTimeStep()), robot(robot)
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
    lidar_range_values.resize(lidar.get_number_of_points());
    point_cloud.resize(lidar.get_number_of_points());
    lidar_fov = lidar.get_fov();
    std::cerr << lidar_resolution << ", " << lidar_max_range << ", " << lidar_fov << std::endl;

    for (int i = 0; i < NUM_SENSORS; ++i) {
        distance_sensors[i] = robot->getDistanceSensor("ps" + std::to_string(i));
        distance_sensors[i]->enable(time_step);
    }
    for (int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = robot->getLED("led" + std::to_string(i));
    }
}

void robot_controller::update_sensor_values()
{
    std::transform(distance_sensors.begin(), distance_sensors.end(), sensor_readings.begin(),
                   [](auto *ds) { return ds->getValue(); });
    
    position = get_position();
    cur_dist2goal = geo::euclidean_dist(position, goal);

    const auto *pointcloud = lidar.get_point_cloud();
    const auto num_points = lidar.get_number_of_points();
    for (int i = 0; i < num_points; ++i) {

        std::cerr << "[" << i << "] (" << pointcloud[i] << ")" << std::endl;
        
        point_cloud[i] = from_lidar_point(pointcloud[i]);

        auto dist = geo::euclidean_dist({0, 0}, point_cloud[i]);
        auto nextval = dist <= 0.95 * lidar_max_range ? std::make_optional(dist) : std::nullopt;
        lidar_range_values[i] = nextval;
    }
}

void robot_controller::run_simulation()
{
    Phase phase = Phase::Motion2Goal;
    set_goal(geo::GlobalPoint{-0.68, 0.79});
    while (robot->step(time_step) != -1) {
        if (!has_goal) {
            stop();
            continue;
        }

        update_sensor_values();
        break;

        if (cur_dist2goal < DIST_TO_GOAL_THRESHOLD) {
            stop();
            continue;
        }

        if (phase == Phase::Motion2Goal) {
            phase = motion2goal();
        }
        else {
            phase = boundary_following();
        }

        /*if (num_steps % 50 == 1)
            tangent_bug_get_destination();*/

        /*if (dist_to_dest < DESTINATION_BUFFER_DISTANCE) {
            stop();
        }
        else if (geo::abs_angle(relative_dest_angle).theta < ANGLE_SENSITIVITY) {
            go_straight_ahead();
        }
        else if (relative_dest_angle.theta > PI) {
            do_right_turn();
        }
        else if (relative_dest_angle.theta < PI) {
            do_left_turn();
        }*/
    }
}

Phase robot_controller::motion2goal() {
    prev_dist2goal = cur_dist2goal;

    // Straight to goal
    geo::Angle angle2goal = get_relative_angle_to_goal();
    int lidar_value_index = get_closest_lidar_point(angle2goal);
    std::optional<double> range2goal = lidar_range_values[lidar_value_index];
    if (!range2goal.has_value()) {
        // Turn towards goal and go forward

        return Phase::Motion2Goal;
    }

    // Around obstacles
    auto discontinuities = get_discontinuity_points();

    double min_heuristic = std::numeric_limits<double>::max();
    geo::GlobalPoint best_point;
    for (const geo::GlobalPoint &point : discontinuities) {
        double h = geo::euclidean_dist(position, point) + geo::euclidean_dist(point, goal);
        if (h < min_heuristic) {
            min_heuristic = h;
            best_point = point;
        }
    }

    if (min_heuristic > prev_dist2goal) {
        return Phase::BoundaryFollowing;
    }
    else {

        // Go to this point
        return Phase::Motion2Goal;
    }

}

Phase robot_controller::boundary_following() {
    return Phase::Motion2Goal;
}

void robot_controller::go_straight_ahead()
{
    // adjust by angle-delta so we approach a direct line.
    //left_motor->setVelocity(6 - 0.1 * relative_dest_angle.theta);
    //right_motor->setVelocity(6 + 0.1 * relative_dest_angle.theta);
    dir = Direction::Straight;
    leds[0]->set(0);
    leds[1]->set(0);
    leds[2]->set(0);
    leds[3]->set(0);
    leds[4]->set(0);
    leds[5]->set(0);
    leds[6]->set(0);
    leds[7]->set(0);
}

void robot_controller::do_left_turn()
{
    left_motor->setVelocity(-4);
    right_motor->setVelocity(4);
    dir = Direction::Left;
    leds[0]->set(0);
    leds[1]->set(1);
    leds[2]->set(1);
    leds[3]->set(1);
    leds[4]->set(0);
    leds[5]->set(0);
    leds[6]->set(0);
    leds[7]->set(0);
}

void robot_controller::do_right_turn()
{
    left_motor->setVelocity(4);
    right_motor->setVelocity(-4);
    dir = Direction::Right;
    leds[0]->set(0);
    leds[1]->set(0);
    leds[2]->set(0);
    leds[3]->set(0);
    leds[4]->set(1);
    leds[5]->set(1);
    leds[6]->set(1);
    leds[7]->set(0);
}

void robot_controller::stop()
{
    left_motor->setVelocity(0);
    right_motor->setVelocity(0);
}

geo::Angle robot_controller::get_facing_angle() const
{
    return geo::angle_of_line(gps_reading_to_point(backGPS), gps_reading_to_point(frontGPS));
}

geo::Angle robot_controller::get_angle_to_goal() const
{
    return geo::angle_of_line(goal, gps_reading_to_point(frontGPS));
}

geo::Angle robot_controller::get_relative_angle_to_goal() const
{
    return get_angle_to_goal() - get_facing_angle();
}

geo::GlobalPoint robot_controller::get_position() const
{
    return geo::get_midpoint(gps_reading_to_point(frontGPS), gps_reading_to_point(backGPS));
}

bool robot_controller::destination_unreachable() const
{
    /*int lidar_idx = get_closest_lidar_point(0);
    if (std::abs(relative_dest_angle.theta) < 0.01)
        return lidar_range_values[lidar_idx].has_value();
    //&& lidar_range_values[lidar_idx].value() < lidar_max_range;
    else {*/
        return false;
    //}
}

std::vector<geo::GlobalPoint> robot_controller::get_discontinuity_points() const
{
    std::vector<geo::GlobalPoint> discontinuities;
    constexpr float THRESHOLD = 0.05f;
    const auto num_points = lidar.get_number_of_points();
    const geo::Angle facing_angle = get_facing_angle();

    for (int i = 0; i < num_points; ++i) {
        int next = (i + 1) % num_points;
        // Switch from object in range to not in range
        if (lidar_range_values[i].has_value() != lidar_range_values[next].has_value()) {
            discontinuities.push_back(
                geo::to_global_coordinates(position, facing_angle.theta, point_cloud[i]));
        }
        // big jump in object distance - assume new object.
        else if (lidar_range_values[i].has_value() && abs(lidar_range_values[i].value() - lidar_range_values[next].value()) > THRESHOLD) {
            discontinuities.push_back(
                geo::to_global_coordinates(position, facing_angle.theta, point_cloud[i]));
        }
    }

    return discontinuities;
}

void robot_controller::tangent_bug_get_destination()
{
    /*auto discontinuities = get_discontinuity_points();
    std::cerr << "bugging\tlen " << discontinuities.size() << std::endl;
    // std::for_each(std::begin(discontinuities), std::end(discontinuities),
    //             [](auto pt) { std::cerr << pt << ' '; });

    double min_heuristic = std::numeric_limits<double>::max();
    geo::GlobalPoint best_point;
    // TODO abort if no discontinuities
    for (const geo::GlobalPoint &point : discontinuities) {
        double h = geo::euclidean_dist(position, point) + geo::euclidean_dist(point, destination);
        if (h < min_heuristic) {
            min_heuristic = h;
            best_point = point;
        }
    }
    std::cerr << "min_heuristic: " << min_heuristic << std::endl;
    std::cerr << "best point " << best_point << std::endl;
    if (min_heuristic > prev_heuristic_dist) {

        // TODO do proper boundary following
        //        throw bad_bug_routing{};
    }
    bug_destination = best_point;
    prev_heuristic_dist = min_heuristic;*/
    // uncomment to enable debug logging to csv file.
    //dump_readings_to_csv();
}
