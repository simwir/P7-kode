#include "robot.hpp"
#include "points.hpp"

#include <algorithm>

robot_controller::robot_controller(webots::Robot *robot)
    : robot(robot), time_step((int)robot->getBasicTimeStep())
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

    for (int i = 0; i < NUM_SENSORS; ++i) {
        distance_sensors[i] = robot->getDistanceSensor("ps" + std::to_string(i));
        distance_sensors[i]->enable(time_step);
    }
}

void robot_controller::run_simulation()
{
    set_destination({0.6, 0, 0.6});

    while (robot->step(time_step) != -1) {
        update_sensor_values();

        printf("angle: %lf\tdest: %lf\tdelta: %lf\n", facing_angle, dest_angle, angle_delta);
        /*std::cout << gps_reading_to_point(frontGPS) << ' ' << gps_reading_to_point(backGPS)
          << std::endl;*/
        if (angle_delta < -ANGLE_SENSITIVITY) {
            do_left_turn();
        }
        else if (angle_delta > ANGLE_SENSITIVITY) {
            do_right_turn();
        }
        else if (dist_to_dest > 0.5) {
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
    angle_delta = dest_angle - facing_angle;
    position = get_position();
    dist_to_dest = has_destination ? euclidean_distance(position, destination) : -1;
}

void robot_controller::go_straight_ahead()
{
    // adjust by angle-delta so we approach a direct line.
    left_motor->setVelocity(6 + angle_delta);
    right_motor->setVelocity(6 - angle_delta);
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
    return get_angle_of_line(destination, gps_reading_to_point(frontGPS));
}

Point robot_controller::get_position() const
{
    return get_average(gps_reading_to_point(frontGPS), gps_reading_to_point(backGPS));
}
