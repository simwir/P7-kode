#ifndef MASTER_HPP
#define MASTER_HPP

#include "config.hpp"
#include "info.hpp"
#include "robot/subscriber.hpp"
#include "scheduling.hpp"
#include "util/euclid.hpp"
#include "wbt-translator/webots_parser.hpp"
#include <tcp/client.hpp>

#include <filesystem>
#include <optional>

const std::filesystem::path dynamic_conf = "dynamic_config.json";
const std::filesystem::path static_conf = "static_config.json";

#define DESTINATION "destination"
#define NEXT_WAYPOINT "next_waypoint"
#define NEXT_STATION "next_station"
#define ROBOT_INFO_MAP "robot_info_map"
#define SELF_STATE "self_state"

#define STATIONS "stations"
#define END_STATIONS "end_stations"
#define VIAS "vias"

constexpr int UPDATE_INTERVAL_MS = 5000;

constexpr int STATION_DELAY = 6;
constexpr int WAYPOINT_DELAY = 3;
constexpr double UNCERTAINTY = 1.1;

namespace robot {
class RecievedMessageException : public std::exception {
    std::string message;

  public:
    RecievedMessageException(const std::string &in_message) : message(in_message) {}

    const char *what() const noexcept override { return message.c_str(); }
};

class CannotOpenFileException : public std::exception {
    std::string message;

  public:
    CannotOpenFileException() : message("Cannot open file") {}
    CannotOpenFileException(const std::string &msg) : message(msg) {}

    const char *what() const noexcept override { return message.c_str(); }
};

class Master {
  public:
    Master(const std::string &robot_host, const std::string &broadcast_host, int robot_id,
           std::istream &world_file);
    void load_webots_to_config();
    void get_dynamic_state();
    void update_dynamic_state();

    void request_broadcast_info();
    void send_robot_info();
    std::string receive_broadcast_info();

    void request_controller_info();
    std::string receive_controller_info();

    std::optional<std::vector<int>> get_new_order();

    void write_static_config();
    void write_dynamic_config();

    void set_robot_destination(int);

    void main_loop();

  private:
    const int id;
    Config static_config;
    Config dynamic_config;
    std::unique_ptr<tcp::Client> webot_client;
    std::unique_ptr<tcp::Client> webots_clock_client;
    tcp::Client broadcast_client;
    Parser webots_parser;
    AST ast;

    std::vector<int> stations;
    std::vector<int> end_stations;
    std::vector<int> vias;
    std::vector<int> waypoints;

    void broadcast_state();
    robot::Info current_state;
    robot::InfoMap robot_info;
    robot::ControllerState controller_state;

    scheduling::StationScheduler station_scheduler;
    scheduling::WaypointScheduler waypoint_scheduler;
    scheduling::EtaExtractor eta_extractor;

    std::shared_ptr<AsyncStationSubscriber> station_subscriber;
    std::shared_ptr<AsyncWaypointSubscriber> waypoint_subscriber;
    std::shared_ptr<AsyncEtaSubscriber> eta_subscriber;

    void set_station_visited(int station)
    {
        if (auto it = std::find(order.begin(), order.end(), station); it != order.end()) {
            order.erase(it);
        }
        visited_waypoints.push_back(station);
    };
    std::vector<int> order;

    // visited waypoints since last station we were at.
    std::vector<int> visited_waypoints;

    bool running;

    int get_webots_time();
    double current_time = 0;
    double eta_start_time = 0;
    double hold_until = 0;

    int last_update_time;

    int get_closest_waypoint(std::function<bool(Waypoint)> pred);
};
} // namespace robot
#endif
