#include "robot.hpp"
#include "geo/geo.hpp"
#include "server.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>

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

int getRobotId(webots::Supervisor *robot){
    auto self = robot->getSelf();
    if (self == nullptr){
        std::cerr << "robot->getSelf() returned 0. Please restart webots";
        exit(1);
    }
    return self->getField("id")->getSFInt32();
}

robot_controller::robot_controller(webots::Supervisor *robot)
    : time_step((int)robot->getBasicTimeStep()),
      robot(robot),
      server(std::to_string(getRobotId(robot)))
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
    // point_cloud.resize(lidar.get_number_of_points());
    point_cloud.resize(lidar.get_number_of_points());
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
    absolute_dest_angle = geo::angle_of_line(gps_reading_to_point(backGPS), get_destination());
    relative_dest_angle = facing_angle - absolute_dest_angle;
    absolute_goal_angle = geo::angle_of_line(gps_reading_to_point(backGPS), destination);
    relative_goal_angle = facing_angle - absolute_goal_angle;

    // angle_to_dest = facing_angle - dest_angle;
    position = get_position();
    dist_to_dest = has_destination ? geo::euclidean_dist(position, destination) : -1;

    const auto *pointcloud = lidar.get_point_cloud();
    const auto num_points = lidar.get_number_of_points();
    for (int i = 0; i < num_points; ++i) {
        point_cloud[i] = from_lidar_point(pointcloud[i]);
        auto dist = geo::euclidean_dist({0, 0}, point_cloud[i]);
        auto nextval = dist <= 0.95 * lidar_max_range ? std::make_optional(dist) : std::nullopt;
        lidar_range_values[i] = nextval;
    }
}

void robot_controller::communicate(){
    for (webots_server::Message message : server.get_messages()){
        switch (message.type){
        case webots_server::MessageType::get_position:
            {
                auto response = std::to_string(position.x) + "," + std::to_string(position.y);
                server.send_message({response, webots_server::MessageType::get_position});
                break;
            }
        case webots_server::MessageType::set_destination:
            {
                size_t split_pos = message.payload.find(",");
                if (split_pos == std::string::npos){
                    server.send_message({message.payload, webots_server::MessageType::not_understood});
                    continue;
                }
                std::cerr << "set destination split." << std::endl;
                destination = {
                                std::stod(message.payload.substr(0,split_pos)),
                                std::stod(message.payload.substr(split_pos + 1))
                };
                break;
            }
        default:
            break;
        }
    }
}

void robot_controller::run_simulation()
{
    set_destination(geo::GlobalPoint{-0.68, 0.79});
    while (robot->step(time_step) != -1) {
        // std::cout << lidar.get_number_of_points() << std::endl;
        num_steps++;
        communicate();
        update_sensor_values();
        /*if (!has_destination)
          break;*/

        // std::cout << "facing angle: " << facing_angle.theta
        //           << "\tabsolute dest angle: " << absolute_dest_angle
        //           << "\trelative dest angle: " << relative_dest_angle
        //           << "\tabsolute goal angle: " << absolute_goal_angle
        //           << "\trelative goal angle: " << relative_goal_angle
        //           << std::endl;
        // std::printf("\tgps:  {x:%lf, z:%lf}\n", frontGPS->getValues()[0],
        // frontGPS->getValues()[2]);

        // if (std::abs(relative_dest_angle.theta - PI) < PI / 2) {
        if (num_steps % 50 == 1)
            tangent_bug_get_destination();
        //}

        /*if (num_steps % 5 != 0) {
            continue;
            }*/

        // std::printf("\tgoal: {x:%lf, y:%lf}", destination.x, destination.y);
        // std::printf("\tdest: {x:%lf, y:%lf}\n", get_destination().x, get_destination().y);

        if (dist_to_dest < DESTINATION_BUFFER_DISTANCE) {
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
        }
    }
}

void robot_controller::go_straight_ahead()
{
    // adjust by angle-delta so we approach a direct line.
    left_motor->setVelocity(6 - 0.1 * relative_dest_angle.theta);
    right_motor->setVelocity(6 + 0.1 * relative_dest_angle.theta);
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
    return geo::angle_of_line(gps_reading_to_point(backGPS), gps_reading_to_point(frontGPS));
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

geo::GlobalPoint robot_controller::get_position() const
{
    return geo::get_average(gps_reading_to_point(frontGPS), gps_reading_to_point(backGPS));
}

bool robot_controller::destination_unreachable() const
{
    int lidar_idx = get_closest_lidar_point(0);
    if (std::abs(relative_dest_angle.theta) < 0.01)
        return lidar_range_values[lidar_idx].has_value();
    //&& lidar_range_values[lidar_idx].value() < lidar_max_range;
    else {
        return false;
    }
}

std::vector<geo::GlobalPoint> robot_controller::get_discontinuity_points() const
{
    // int _i = 0;
    // std::for_each(std::begin(lidar_range_values), std::end(lidar_range_values), [&_i](auto val) {
    //     if (val.has_value())
    //         std::cout << val.value() << ' ';
    //     ++_i;
    // });
    std::vector<geo::GlobalPoint> discontinuities;

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
            std::cerr << "switched in_range \n";
            is_in_range = !is_in_range;
            discontinuities.push_back(
                geo::to_global_coordinates(position, facing_angle, point_cloud[i]));
        }
        // big jump in object distance - assume new object.
        else if (is_in_range && abs(lidar_range_values[i].value() - last_range_value) > THRESHOLD) {
            std::cerr << "object switch found \n";
            // std::cerr << lidar_value_to_point(i) << '\t' << lidar_value_to_point(i - 1);
            discontinuities.push_back(
                geo::to_global_coordinates(position, facing_angle, point_cloud[i]));
            discontinuities.push_back(
                geo::to_global_coordinates(position, facing_angle, point_cloud[i - 1]));
        }
        // TODO not working!
        // add point in direct path to destination if possible
        else if ((!lidar_range_values[i].has_value() ||
                  lidar_range_values[i].value() < dist_to_dest)) {
            auto global_point = geo::to_global_coordinates(position, facing_angle, point_cloud[i]);
            auto angle_diff =
                geo::abs_angle(absolute_goal_angle - geo::angle_of_line(position, global_point))
                    .theta;
            // std::cerr << "angle_diff " << angle_diff << '\n';
            //TODO logic if goal is closer than lidar point
            if (angle_diff <= PI / (lidar.get_number_of_points())) {
                auto tangent_point =
                    geo::to_global_coordinates(position, facing_angle, point_cloud[i]);
                std::cerr << "tangent point inserted " << tangent_point << std::endl;
                discontinuities.push_back(tangent_point);
            }
        }
        if (is_in_range) {
            last_range_value = lidar_range_values[i].value();
        }
    }
    std::cerr << std::endl;
    return discontinuities;
}

void robot_controller::tangent_bug_get_destination()
{
    auto discontinuities = get_discontinuity_points();
    std::cerr << "bugging\tlen " << discontinuities.size() << std::endl;
    // std::for_each(std::begin(discontinuities), std::end(discontinuities),
    //             [](auto pt) { std::cerr << pt << ' '; });

    double min_heuristic = std::numeric_limits<double>::max();
    geo::GlobalPoint best_point;
    //TODO abort if no discontinuities
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
    prev_heuristic_dist = min_heuristic;
    dump_readings_to_csv();
}
