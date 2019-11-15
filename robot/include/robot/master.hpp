#ifndef MASTER_HPP
#define MASTER_HPP

#include "config.hpp"
#include "info.hpp"
#include "robot/subscriber.hpp"
#include "scheduling.hpp"
#include "wbt-translator/webots_parser.hpp"
#include "util/euclid.hpp"
#include "util/logger.hpp"
#include <tcp/client.hpp>

#include <filesystem>
#include <optional>

const std::filesystem::path dynamic_conf = "dynamic_config.json";
const std::filesystem::path static_conf = "static_config.json";

#define DESTINATION "destination"
#define CURRENT_WAYPOINT "current_waypoint"
#define CURRENT_STATION "current_station"
#define ROBOT_INFO_MAP "robot_info_map"
#define SELF_STATE "self_state"

#define STATIONS "stations"
#define END_STATIONS "end_stations"
#define VIAS "vias"
#define NUMBER_OF_STATIONS "number_of_stations"
#define NUMBER_OF_END_STATIONS "number_of_end_stations"
#define NUMBER_OF_VIAS "number_of_vias"
#define NUMBER_OF_WAYPOINTS "number_of_waypoints"


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

    bool running;

    double get_webots_time();
    double current_webots_time = 0;
    double eta_start_time = 0;
    double hold_untill = 0;

    int get_closest_waypoint(std::function<bool(Waypoint)> pred);
};
} // namespace robot
#endif
