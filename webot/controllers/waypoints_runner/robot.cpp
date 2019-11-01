#include "robot.hpp"
#include "geo/geo.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>

constexpr double DIST_TO_GOAL_THRESHOLD = 0.1;
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

RobotController::RobotController(webots::Supervisor *robot)
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
    lidar_min_range = lidar.get_min_range();
    lidar_range_values.resize(lidar_resolution);
    //point_cloud.resize(lidar_resolution);
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
    const auto quarter = lidar_resolution / 4;
    for (int i = 0; i < lidar_resolution; ++i) {
        const auto index = lidar_resolution - 1 - (i - quarter + lidar_resolution) % lidar_resolution;
        
        auto dist = range_image[i];
        auto nextval = 1.01 * lidar_min_range <= dist && dist <= 0.99 * lidar_max_range ? std::make_optional(dist) : std::nullopt;
        
        lidar_range_values[index] = nextval;
    }
}

void RobotController::run_simulation()
{
    set_goal(geo::GlobalPoint{-0.68, 0.79});
    while (robot->step(time_step) != -1) {
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

Phase RobotController::motion2goal() {
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
        return Phase::BoundaryFollowing;
    }
    else {
        geo::Angle angle = get_angle_to_point(best_point);
        std::cerr << "Towards discontinuity: " << angle << ", " << (dir == DiscontinuityDirection::Left) << std::endl;
        const auto dist = geo::euclidean_dist(position, best_point);
        const auto rectified_remaining_dist = (1 - std::min(1.0, dist));
        const auto angle_correction = rectified_remaining_dist * PI / 4 *
            (dir == DiscontinuityDirection::Left ? 1 : -1);
        
        go_towards_angle(angle + angle_correction);
        
        return Phase::Motion2Discontinuity;
    }
}

Phase RobotController::boundary_following() {
    return Phase::Motion2Goal;
}

bool RobotController::clear() {
    if (phase == Phase::Motion2Goal) {
        return true;
    }
  
    // const auto num_points = lidar.get_number_of_points();
    for (int i = 0; i < lidar_resolution; ++i) {
        const auto angle = index2angle(i);
        const auto diff = abs_angle(angle - get_relative_angle_to_goal());
        
        if (diff.theta <= PI / 2 && 
            lidar_range_values[i].has_value() && 
            geo::RelPoint::from_polar(lidar_range_values[i].value(), diff).x <= ROBOT_RADIUS) {
            return false;
        }
    }
    
    return true;
}

void RobotController::go_towards_angle(const geo::Angle& angle) {
    geo::Angle abs = geo::abs_angle(angle);
    
    if (angle.theta > PI / 10) {
        std::cerr << "Turning left" << std::endl;
        do_left_turn();
    }
    else if (angle.theta < -PI / 10) {
        std::cerr << "Turning right" << std::endl;
        do_right_turn();
    }
    else if (abs.theta < 0.5 * (PI / 180)) { // If we are off by less than 0.5 degrees, we will not perform any correction
        std::cerr << "Going straight ahead" << std::endl;
        go_straight_ahead();
    }
    else {
        std::cerr << "Going straight with adjustment" << std::endl;
        go_adjusted_straight(angle);
    }
}

void RobotController::go_adjusted_straight(const geo::Angle& angle)
{
    left_motor->setVelocity(8 - 3.5 * angle.theta);
    right_motor->setVelocity(8 + 3.5 * angle.theta);
    set_leds(Direction::Straight);
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
    set_leds(Direction::Left);
}

void RobotController::do_right_turn()
{
    left_motor->setVelocity(4);
    right_motor->setVelocity(-4);
    set_leds(Direction::Right);
}

void RobotController::set_leds(Direction dir) {
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

geo::Angle RobotController::get_angle_to_point(const geo::GlobalPoint& point) const
{
    geo::Angle angle = geo::angle_of_line(get_position(), point);
    return angle - get_facing_angle();
}

geo::GlobalPoint RobotController::get_position() const
{
    return geo::get_midpoint(gps_reading_to_point(frontGPS), gps_reading_to_point(backGPS));
}

std::vector<std::pair<geo::GlobalPoint, DiscontinuityDirection>> RobotController::get_discontinuity_points() const
{
    std::vector<std::pair<geo::GlobalPoint, DiscontinuityDirection>> discontinuities;
    constexpr float THRESHOLD = 0.05f;
    const geo::Angle facing_angle = get_facing_angle();

    for (int i = 0; i < lidar_resolution; ++i) {
        int next = (i + 1) % lidar_resolution;
        // Switch from object in range to not in range
        if (lidar_range_values[i].has_value() != lidar_range_values[next].has_value()) {
            const auto dist = lidar_range_values[i].has_value() ? lidar_range_values[i].value() : lidar_max_range;
            const auto angle = index2angle(i);
            
            auto point = geo::RelPoint::from_polar(dist, angle);
            auto direction = DiscontinuityDirection::Right;
            
            if (lidar_range_values[i].has_value()) {
              direction = DiscontinuityDirection::Left;
            }
            
            discontinuities.push_back(std::make_pair(
                geo::to_global_coordinates(position, facing_angle, point),
                direction
            ));
        }
        // big jump in object distance - assume new object.
        else if (lidar_range_values[i].has_value() && abs(lidar_range_values[i].value() - lidar_range_values[next].value()) > THRESHOLD) {
            const auto angle = index2angle(i);
            
            auto point = geo::RelPoint::from_polar(lidar_range_values[i].value(), angle);
            auto direction = DiscontinuityDirection::Right;
            
            if (lidar_range_values[i].value() < lidar_range_values[next].value()) {
              direction = DiscontinuityDirection::Left;
            }
            
            discontinuities.push_back(std::make_pair(
                geo::to_global_coordinates(position, facing_angle, point),
                direction
            ));
        }
    }

    return discontinuities;
}
