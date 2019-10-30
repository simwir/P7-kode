#ifndef MY_ROBOT_HPP
#define MY_ROBOT_HPP

#include <webots/DistanceSensor.hpp>
#include <webots/GPS.hpp>
#include <webots/Lidar.hpp>
#include <webots/Motor.hpp>
#include <webots/Robot.hpp>
#include <webots/Supervisor.hpp>
#include <webots/LED.hpp>

#include <array>
#include <optional>
#include <stdexcept>
#include <vector>

//#include "points.hpp"
#include "lidar_wrapper.hpp"
#include "geo/geo.hpp"

constexpr auto NUM_SENSORS = 8;
constexpr auto NUM_LEDS = 8;
constexpr auto ANGLE_SENSITIVITY = 0.8;
constexpr auto DESTINATION_BUFFER_DISTANCE = 0.05;


enum class Direction {Left, Right, Straight};
enum class Phase {Motion2Goal, BoundaryFollowing};

struct DestinationNotDefinedException {
};

struct bad_bug_routing : std::exception {
    const char *what() const noexcept override { return "bad_bug_routing"; }
};

class robot_controller {
  public:
    robot_controller(webots::Supervisor *robot);
    ~robot_controller() { delete robot; }

    void run_simulation();
    Phase motion2goal();
    Phase boundary_following();

    void set_goal(const geo::GlobalPoint &point)
    {
        goal = point;
        has_goal = true;
    }

    void abort_goal() { has_goal = false; }

    geo::GlobalPoint get_position() const;

    geo::Angle get_facing_angle() const;
    geo::Angle get_angle_to_goal() const;
    geo::Angle get_relative_angle_to_goal() const;

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
    lidar_wrapper lidar;
    std::array<webots::DistanceSensor *, NUM_SENSORS> distance_sensors;
    std::array<webots::LED *, NUM_LEDS> leds;

    int lidar_resolution;
    double lidar_max_range;
    int lidar_num_layers = 1;
    double lidar_fov;
    double range_threshold;
    inline size_t lidar_num_points() { return lidar_resolution * lidar_num_layers; }
    std::vector<std::optional<double>> lidar_range_values;
    std::vector<geo::RelPoint> point_cloud;

    bool has_goal = false;
    geo::GlobalPoint goal;
    std::optional<geo::GlobalPoint> bug_destination = std::nullopt;

    inline geo::GlobalPoint get_destination() const
    {
        if (bug_destination.has_value()) {
            return bug_destination.value();
        }
        else
            return goal;
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

    std::vector<geo::GlobalPoint> get_discontinuity_points() const;

    double prev_heuristic_dist = std::numeric_limits<double>::max();

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

    double cur_dist2goal;
    double prev_dist2goal;

    geo::GlobalPoint position;
    Direction dir;

    size_t num_steps = 0;

    void dump_readings_to_csv(const std::string& pcfilename = "point_cloud.csv",
                              const std::string& rangefilename = "range_values.csv");

};

#endif
