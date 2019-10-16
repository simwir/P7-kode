#include "robot.hpp"
#include "points.hpp"

#include <algorithm>
#include <optional>

robot_controller::robot_controller(webots::Robot *robot)
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

    lidar = robot->getLidar("lidar");
    lidar->enable(time_step);
    lidar_resolution = lidar->getHorizontalResolution();
    lidar_max_range = lidar->getMaxRange();
    lidar_num_layers = lidar->getNumberOfLayers();
    lidar_range_values.reserve(lidar_num_points());
    lidar_fov = lidar->getFov();

    for (int i = 0; i < NUM_SENSORS; ++i) {
        distance_sensors[i] = robot->getDistanceSensor("ps" + std::to_string(i));
        distance_sensors[i]->enable(time_step);
    }
}

void robot_controller::run_simulation()
{
    set_destination({-0.68, 0, 0.79});

    while (robot->step(time_step) != -1) {
        update_sensor_values();
        if (!has_destination)
            break;

        if (destination_unreachable()) {
            tangent_bug_get_destination();
        }

        printf("angle: %lf\tdest: %lf\tdelta: %lf\n", facing_angle, dest_angle, angle_delta);
        /*std::cout << gps_reading_to_point(frontGPS) << ' ' << gps_reading_to_point(backGPS)
          << std::endl;*/
        if (angle_delta < -ANGLE_SENSITIVITY) {
            do_right_turn();
        }
        else if (angle_delta > ANGLE_SENSITIVITY) {
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

void robot_controller::update_sensor_values()
{
    std::transform(distance_sensors.begin(), distance_sensors.end(), sensor_readings.begin(),
                   [](auto *ds) { return ds->getValue(); });
    facing_angle = get_facing_angle();
    dest_angle = get_angle_to_dest();
    angle_delta =
        std::atan2(std::sin(facing_angle - dest_angle), std::cos(facing_angle - dest_angle));
    position = get_position();
    dist_to_dest = has_destination ? euclidean_distance(position, destination) : -1;

    const auto *vals = lidar->getRangeImage();
    std::transform(
        vals, vals + lidar_num_points(), std::begin(lidar_range_values), [&](auto reading) {
            return reading > lidar_max_range ? std::nullopt : std::make_optional(reading);
        });
}

void robot_controller::go_straight_ahead()
{
    // adjust by angle-delta so we approach a direct line.
    left_motor->setVelocity(6 - angle_delta);
    right_motor->setVelocity(6 + angle_delta);
}

void robot_controller::do_left_turn()
{
    left_motor->setVelocity(-2);
    right_motor->setVelocity(4);
}

void robot_controller::do_right_turn()
{
    left_motor->setVelocity(4);
    right_motor->setVelocity(-2);
}

void robot_controller::stop()
{
    left_motor->setVelocity(0);
    right_motor->setVelocity(0);
}
double robot_controller::get_facing_angle() const
{
    return get_angle_of_line(gps_reading_to_point(frontGPS), gps_reading_to_point(backGPS));
}

double robot_controller::get_angle_to_dest() const
{
    if (!has_destination)
        throw DestinationNotDefinedException{};
    return get_angle_of_line(get_destination(), gps_reading_to_point(frontGPS));
}

Point robot_controller::get_position() const
{
    return get_average(gps_reading_to_point(frontGPS), gps_reading_to_point(backGPS));
}

bool robot_controller::destination_unreachable() const
{
    int lidar_idx = get_closest_lidar_point(0);
    return lidar_range_values[lidar_idx].has_value();
}

std::vector<Point> robot_controller::get_discontinuity_points() const
{
    std::vector<Point> discontinuities;
    bool is_in_range = lidar_range_values[0].has_value();
    for (size_t i = 0; i < lidar_range_values.size(); ++i) {
        if (is_in_range != lidar_range_values[i].has_value()) {
            is_in_range = !is_in_range;
            discontinuities.push_back(
                get_coordinate_system().to_global_coordinates(lidar_value_to_point(i)));
        }
    }
    return discontinuities;
}

void robot_controller::tangent_bug_get_destination()
{
    std::cerr << "bugging\n";
    auto discontinuities = get_discontinuity_points();
    double min_heuristic = std::numeric_limits<double>::max();
    Point best_point;
    for (auto& point : discontinuities) {
        double h = euclidean_distance(position, point) + euclidean_distance(point, destination);
        if (h < min_heuristic) {
            min_heuristic = h;
            best_point = point;
        }
    }
    if (min_heuristic > prev_heuristic_dist) {
        // TODO do proper boundary following
        throw bad_bug_routing{};
    }
    bug_destination = best_point;
    prev_heuristic_dist = min_heuristic;
}
