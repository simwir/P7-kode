#ifndef MY_ROBOT_HPP
#define MY_ROBOT_HPP

#include <webots/DistanceSensor.hpp>
#include <webots/GPS.hpp>
#include <webots/Lidar.hpp>
#include <webots/Motor.hpp>
#include <webots/Robot.hpp>

#include <array>
#include <optional>
#include <stdexcept>
#include <vector>

#include "points.hpp"

constexpr auto NUM_SENSORS = 8;
constexpr auto ANGLE_SENSITIVITY = 0.1;
constexpr auto DESTINATION_BUFFER_DISTANCE = 0.2;

struct DestinationNotDefinedException {
};

struct bad_bug_routing : std::exception {
    const char *what() const noexcept override { return "bad_bug_routing"; }
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
    webots::Lidar *lidar;
    std::array<webots::DistanceSensor *, NUM_SENSORS> distance_sensors;

    int lidar_resolution;
    int lidar_max_range;
    int lidar_num_layers;
    double lidar_fov;
    double range_threshold;
    inline size_t lidar_num_points() { return lidar_resolution * lidar_num_layers; }
    std::vector<std::optional<float>> lidar_range_values;

    bool has_destination = false;
    Point destination;
    std::optional<Point> bug_destination = std::nullopt;

    inline Point get_destination() const
    {
        if (bug_destination.has_value()) {
            return bug_destination.value();
        }
        else
            return destination;
    }

    float get_lidar_point_angle(size_t idx) const
    {
        // off by one?
        return idx * (lidar_fov / lidar_resolution) - lidar_fov / 2;
    }

    // angle relative to facing angle
    int get_closest_lidar_point(float angle) const
    {
        return std::round((angle + lidar_fov / 2) * lidar_resolution / lidar_fov);
    }

    Point lidar_value_to_point(int idx) const
    {
        double dist =
            lidar_range_values[idx].has_value() ? lidar_range_values[idx].value() : lidar_max_range;
        double angle = get_lidar_point_angle(idx);

        return {dist * std::cos(angle), 0, dist * std::sin(angle)};
    }

    std::vector<Point> get_discontinuity_points() const;

    double prev_heuristic_dist = std::numeric_limits<double>::max();

    CoordinateSystem get_coordinate_system() const { return {position, facing_angle}; }

    // actions
    void do_left_turn();
    void do_right_turn();
    void go_straight_ahead();
    void stop();

    void update_sensor_values();
    bool destination_unreachable() const;
    void tangent_bug_get_destination();

    // current state inforamtion
    std::array<double, NUM_SENSORS> sensor_readings;
    double dist_to_dest;
    double facing_angle;
    double dest_angle;
    double angle_delta;
    Point position;
};

#endif
