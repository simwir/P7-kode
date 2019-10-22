#include "robot.hpp"

#include <algorithm>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <iostream>

template <typename Container>
static void dump_to_csv(const Container &container)
{
    std::ofstream csv{"/home/waefwerf/dev/P7/webot/point_cloud.csv"};
    for (auto pt : container) {
        csv << pt << std::endl;
    }
}

std::ostream &operator<<(std::ostream &os, const webots::LidarPoint &point)
{
    return os << point.x << ", " << point.z;
}

geo::Point from_lidar_point(const webots::LidarPoint &point)
{
    return {point.x, point.z};
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
    lidar_point_cloud.resize(lidar.get_number_of_points());
    lidar_fov = lidar.get_fov();

    for (int i = 0; i < NUM_SENSORS; ++i) {
        distance_sensors[i] = robot->getDistanceSensor("ps" + std::to_string(i));
        distance_sensors[i]->enable(time_step);
    }
}

void robot_controller::update_sensor_values()
{
    std::transform(distance_sensors.begin(), distance_sensors.end(), sensor_readings.begin(),
                   [](auto *ds) { return ds->getValue(); });
    facing_angle = get_facing_angle();
    dest_angle = get_angle_to_dest();
    angle_to_dest = facing_angle - dest_angle;
    position = get_position();
    dist_to_dest = has_destination ? geo::euclidean_dist(position, destination) : -1;

    const auto *range_image = lidar.get_range_image();
    const auto *point_cloud = lidar.get_point_cloud();
    const auto num_points = lidar.get_number_of_points();
    for (int i = 0; i < num_points; ++i) {
        lidar_point_cloud[i] = point_cloud[i];
        auto dist = geo::euclidean_dist(position, {lidar_point_cloud[i].x, lidar_point_cloud[i].y});
        lidar_range_values[i] =
            dist >= lidar_max_range * 0.9 ? std::make_optional(dist) : std::nullopt;
    }
}

void robot_controller::run_simulation()
{
    set_destination(geo::Point{-0.68, 0.79});
    while (robot->step(time_step) != -1) {
        num_steps++;
        update_sensor_values();
        /*if (!has_destination)
          break;*/

        std::cout << "angle: " << facing_angle.theta << "\tdest: " << dest_angle.theta << "\tdelta: "  << angle_to_dest.theta << std::endl;
        std::printf("\tdest: {x:%lf, z:%lf}", destination.x, destination.y);
        std::printf("\tgps:  {x:%lf, z:%lf}", frontGPS->getValues()[0], frontGPS->getValues()[2]);

        // if (std::abs(angle_to_dest.theta - PI) < PI / 2) {
        if (num_steps % 50 == 1)
            tangent_bug_get_destination();
        //}

        if (num_steps % 5 == 0) {
            continue;
        }

        if (angle_to_dest.theta > PI && angle_to_dest.theta < 2 * PI - ANGLE_SENSITIVITY) {
            do_right_turn();
        }
        else if (angle_to_dest.theta > ANGLE_SENSITIVITY) {
            do_left_turn();
        }
        else if (dist_to_dest > DESTINATION_BUFFER_DISTANCE) {
            go_straight_ahead();
        }
        else {
            stop();
        }
    }
}

void robot_controller::go_straight_ahead()
{
    // adjust by angle-delta so we approach a direct line.
    left_motor->setVelocity(6 - 0.1 * angle_to_dest.theta);
    right_motor->setVelocity(6 + 0.1 * angle_to_dest.theta);
}

void robot_controller::do_left_turn()
{
    left_motor->setVelocity(-4);
    right_motor->setVelocity(4);
}

void robot_controller::do_right_turn()
{
    left_motor->setVelocity(4);
    right_motor->setVelocity(-4);
}

void robot_controller::stop()
{
    left_motor->setVelocity(0);
    right_motor->setVelocity(0);
}
geo::Angle robot_controller::get_facing_angle() const
{
    return geo::angle_of_line(gps_reading_to_point(frontGPS), gps_reading_to_point(backGPS));
}

geo::Angle robot_controller::get_angle_to_dest() const
{
    if (!has_destination)
        throw DestinationNotDefinedException{};
    return geo::angle_of_line(get_destination(), gps_reading_to_point(frontGPS));
}

geo::Angle robot_controller::get_angle_to_goal() const
{
    return geo::angle_of_line(destination, gps_reading_to_point(frontGPS));
}

geo::Point robot_controller::get_position() const
{
    return geo::get_average(gps_reading_to_point(frontGPS), gps_reading_to_point(backGPS));
}

bool robot_controller::destination_unreachable() const
{
    int lidar_idx = get_closest_lidar_point(0);
    if (std::abs(angle_to_dest.theta) < 0.01)
        return lidar_range_values[lidar_idx].has_value();
    //&& lidar_range_values[lidar_idx].value() < lidar_max_range;
    else {
        return false;
    }
}

std::vector<geo::Point> robot_controller::get_discontinuity_points() const
{
    // int _i = 0;
    // std::for_each(std::begin(lidar_range_values), std::end(lidar_range_values), [&_i](auto val) {
    //     if (val.has_value())
    //         std::cout << val.value() << ' ';
    //     ++_i;
    // });
    std::vector<geo::Point> discontinuities;

    // refactoring target
    /*discontinuities.push_back(to_global_coordinates(
      lidar_value_to_point(get_closest_lidar_point(get_angle_to_goal()))));*/

    bool is_in_range = lidar_range_values[0].has_value();
    constexpr float THRESHOLD = 0.05f;
    float last_range_value =
        is_in_range ? lidar_range_values[0].value() : std::numeric_limits<float>::max();
    for (size_t i = 1; i < lidar_range_values.size(); ++i) {
        // switch from object in range to not in range
        if (is_in_range != lidar_range_values[i].has_value()) {
            is_in_range = !is_in_range;
            discontinuities.push_back(geo::to_global_coordinates(
                position, facing_angle, from_lidar_point(lidar_point_cloud[i])));
        }
        // big jump in object distance - assume new object.
        else if (is_in_range && abs(lidar_range_values[i].value() - last_range_value) > THRESHOLD) {
            std::cerr << lidar_value_to_point(i) << '\t' << lidar_value_to_point(i - 1);
            discontinuities.push_back(geo::to_global_coordinates(
                position, facing_angle, from_lidar_point(lidar_point_cloud[i])));
            discontinuities.push_back(geo::to_global_coordinates(
                position, facing_angle, from_lidar_point(lidar_point_cloud[i - 1])));
        }
        if (is_in_range) {
            last_range_value = lidar_range_values[i].value();
        }
    }
    return discontinuities;
}

void robot_controller::tangent_bug_get_destination()
{
    auto discontinuities = get_discontinuity_points();
    std::cerr << "bugging\tlen " << discontinuities.size() << std::endl;
    // std::for_each(std::begin(discontinuities), std::end(discontinuities),
    // Y              [](auto pt) { std::cerr << pt << ' '; });

    double min_heuristic = std::numeric_limits<double>::max();
    geo::Point best_point;
    for (auto &point : discontinuities) {
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
    prev_heuristic_dist = min_heuristic;
    dump_to_csv(lidar_point_cloud);
}
