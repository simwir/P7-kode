#ifndef MY_ROBOT_HPP
#define MY_ROBOT_HPP

#include <webots/DistanceSensor.hpp>
#include <webots/GPS.hpp>
#include <webots/Motor.hpp>
#include <webots/Robot.hpp>

#include <array>

#include "points.hpp"

constexpr auto NUM_SENSORS = 8;
constexpr auto ANGLE_SENSITIVITY = 0.1;
constexpr auto DESTINATION_BUFFER_DISTANCE = 0.2;

struct DestinationNotDefinedException {
};

class robot_controller {
  public:
    robot_controller(webots::Robot *robot);

    void run_simulation();

    void set_destination(const Point &point)
    {
        destination = point;
        has_destination = true;
    }

    void abort_destination() { has_destination = false; }

    ~robot_controller() { delete robot; }

    double get_facing_angle() const;

    Point get_position() const;

    double get_angle_to_dest() const;

    static Point gps_reading_to_point(const webots::GPS *gps)
    {
        const double *reading = gps->getValues();
        return {reading[0], reading[1], reading[2]};
    }

  private:
    const int time_step;

    webots::Robot *robot;
    webots::GPS *frontGPS, *backGPS;
    webots::Motor *left_motor, *right_motor;
    std::array<webots::DistanceSensor *, NUM_SENSORS> distance_sensors;

    bool has_destination = false;
    Point destination;

    // actions
    void do_left_turn();
    void do_right_turn();
    void go_straight_ahead();
    void stop();

    void update_sensor_values();

    // current state inforamtion
    std::array<double, NUM_SENSORS> sensor_readings;
    double dist_to_dest;
    double facing_angle;
    double dest_angle;
    double angle_delta;
    Point position;
};

#endif
