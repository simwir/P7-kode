#ifndef MY_ROBOT_HPP
#define MY_ROBOT_HPP

#include <webots/DistanceSensor.hpp>
#include <webots/GPS.hpp>
#include <webots/LED.hpp>
#include <webots/Lidar.hpp>
#include <webots/Motor.hpp>
#include <webots/Robot.hpp>
#include <webots/Supervisor.hpp>

#include <array>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "geo/geo.hpp"
#include "lidar_wrapper.hpp"
#include "webots_server.hpp"

constexpr auto NUM_SENSORS = 8;
constexpr auto NUM_LEDS = 8;
constexpr auto ANGLE_SENSITIVITY = 0.8;
constexpr auto DESTINATION_BUFFER_DISTANCE = 0.05;

enum class Direction { Left, Right, Straight };
enum class Phase { Motion2Goal, Motion2Discontinuity, BoundaryFollowing };
enum class DiscontinuityDirection { Left, Right };

class RobotController {
  public:
    RobotController(webots::Supervisor *robot);
    ~RobotController() { delete robot; }

    void run_simulation();
    Phase motion2goal();
    Phase boundary_following();

    void communicate();

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
    geo::Angle get_angle_to_point(const geo::GlobalPoint &point) const;
    void update_dfollowed();
    bool clear();
    double dreach();
    geo::Angle normal_angle();

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
    LidarWrapper lidar;
    std::array<webots::DistanceSensor *, NUM_SENSORS> distance_sensors;
    std::array<webots::LED *, NUM_LEDS> leds;

    int lidar_resolution;
    double lidar_max_range;
    double lidar_min_range;
    int lidar_num_layers = 1;
    double lidar_fov;
    double range_threshold;
    size_t lidar_num_points() { return lidar_resolution * lidar_num_layers; }
    std::vector<std::optional<double>> lidar_range_values;
    std::vector<geo::RelPoint> point_cloud;

    webots_server::Server server;

    bool is_stopped = true;
    bool has_goal = false;
    geo::GlobalPoint goal;

    int angle2index(const geo::Angle &angle) const
    {
        return std::round(((angle.theta + PI) / (2 * PI)) * lidar_resolution);
    }

    geo::Angle index2angle(int index) const
    {
        const double angle = (static_cast<double>(index) / lidar_resolution) * 2 * PI - PI;
        return geo::Angle{angle};
    }

    std::vector<std::pair<geo::GlobalPoint, DiscontinuityDirection>>
    get_discontinuity_points() const;
    void go_towards_angle(const geo::Angle &angle);
    void go_to_discontinuity(geo::GlobalPoint point, DiscontinuityDirection dir);

    // actions
    void do_left_turn();
    void do_right_turn();
    void go_straight_ahead();
    void go_adjusted_straight(const geo::Angle &angle);
    void stop();
    void set_leds(Direction dir);

    // current state information
    std::array<double, NUM_SENSORS> sensor_readings;

    double cur_dist2goal;
    double prev_dist2goal;
    bool first_iteration = true;
    Phase phase = Phase::Motion2Goal;
    double dfollowed;

    geo::GlobalPoint position;

    void dump_readings_to_csv(const std::string &pcfilename = "point_cloud.csv",
                              const std::string &rangefilename = "range_values.csv");
};

#endif
