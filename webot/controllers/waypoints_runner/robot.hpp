#ifndef MY_ROBOT_HPP
#define MY_ROBOT_HPP

#include <webots/DistanceSensor.hpp>
#include <webots/GPS.hpp>
#include <webots/Lidar.hpp>
#include <webots/Motor.hpp>
#include <webots/Robot.hpp>
#include <webots/Supervisor.hpp>

#include <array>
#include <optional>
#include <stdexcept>
#include <vector>

//#include "points.hpp"
#include "geo/geo.hpp"
#include "lidar_wrapper.hpp"
#include "webots_server.hpp"

constexpr auto NUM_SENSORS = 8;
constexpr auto ANGLE_SENSITIVITY = 0.8;
constexpr auto DESTINATION_BUFFER_DISTANCE = 0.05;

struct DestinationNotDefinedException {
};

struct bad_bug_routing : std::exception {
    const char *what() const noexcept override { return "bad_bug_routing"; }
};

class robot_controller {
  public:
    robot_controller(webots::Supervisor *robot);

    void run_simulation();

    void communicate();

    void set_destination(const geo::GlobalPoint &point)
    {
        destination = point;
        has_destination = true;
    }

    void abort_destination() { has_destination = false; }

    ~robot_controller() { delete robot; }

    geo::Angle get_facing_angle() const;

    geo::GlobalPoint get_position() const;

    geo::Angle get_angle_to_dest() const;
    geo::Angle get_angle_to_goal() const;

    static geo::GlobalPoint gps_reading_to_point(const webots::GPS *gps)
    {
        const double *reading = gps->getValues();
        return geo::GlobalPoint{reading[0], reading[2]};
    }

    void update_sensor_values();

  private:
    const int time_step;

    webots::Supervisor *robot;
    webots::GPS *frontGPS, *backGPS;
    webots::Motor *left_motor, *right_motor;
    webots::Lidar *_lidar;
    lidar_wrapper lidar;
    std::array<webots::DistanceSensor *, NUM_SENSORS> distance_sensors;

    int lidar_resolution;
    double lidar_max_range;
    int lidar_num_layers = 1;
    double lidar_fov;
    double range_threshold;
    inline size_t lidar_num_points() { return lidar_resolution * lidar_num_layers; }
    std::vector<std::optional<double>> lidar_range_values;
    std::vector<geo::RelPoint> point_cloud;
    //    std::vector<webots::LidarPoint> lidar_point_cloud;

    webots_server::Server server;

    bool is_stopped = true;
    bool has_destination = false;
    geo::GlobalPoint destination;
    std::optional<geo::GlobalPoint> bug_destination = std::nullopt;

    inline geo::GlobalPoint get_destination() const
    {
        if (bug_destination.has_value()) {
            return bug_destination.value();
        }
        else
            return destination;
    }

    // float get_lidar_point_angle(size_t idx) const
    // {
    //     // off by one?
    //     return idx * (lidar_fov / lidar_resolution) - lidar_fov / 2;
    // }

    // angle relative to facing angle
    int get_closest_lidar_point(geo::Angle angle) const
    {
        return std::round((angle.theta + lidar_fov / 2) * lidar_resolution / lidar_fov);
    }

    // geo::Point lidar_value_to_point(int idx) const
    // {
    //     /*double dist =
    //         lidar_range_values[idx].has_value() ? lidar_range_values[idx].value() :
    //         lidar_max_range;
    //     double angle = get_lidar_point_angle(idx);

    //     return {dist * std::cos(angle), 0, dist * std::sin(angle)};*/
    //     const auto pt = lidar_point_cloud[idx];
    //     return geo::Point{pt.x, pt.z};
    // }

    std::vector<geo::GlobalPoint> get_discontinuity_points() const;

    double prev_heuristic_dist = std::numeric_limits<double>::max();

    // CoordinateSystem get_coordinate_system() const { return {position, facing_angle}; }

    // actions
    void do_left_turn();
    void do_right_turn();
    void go_straight_ahead();
    void stop();

    bool destination_unreachable() const;
    void tangent_bug_get_destination();

    // current state information
    std::array<double, NUM_SENSORS> sensor_readings;
    double dist_to_dest;
    geo::Angle facing_angle;

    geo::Angle absolute_goal_angle;
    geo::Angle absolute_dest_angle;

    geo::Angle relative_goal_angle;
    geo::Angle relative_dest_angle;

    // geo::Angle dest_angle;
    // geo::Angle angle_to_dest;
    geo::GlobalPoint position;

    size_t num_steps = 0;

    void dump_readings_to_csv(const std::string &pcfilename = "point_cloud.csv",
                              const std::string &rangefilename = "range_values.csv");
};

#endif
