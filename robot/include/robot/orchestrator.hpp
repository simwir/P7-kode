/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler
 *Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 *associated documentation files (the "Software"), to deal in the Software without restriction,
 *including without limitation the rights to use, copy, modify, merge, publish, distribute,
 *sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or
 *substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 *NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
 *OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef ORCHESTRATOR_HPP
#define ORCHESTRATOR_HPP

#include "config/config.hpp"
#include "robot/clock.hpp"
#include "robot/info.hpp"
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

class Orchestrator {
  public:
    Orchestrator(const std::string &robot_host, const std::string &broadcast_host, int robot_id,
                 std::istream &world_file, const std::string &clock_host = "127.0.0.1");
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

    void add_waypoint_matrix(const AST &ast);
    void add_station_matrix(const AST &ast);
    void dump_waypoint_info(const AST &ast);

  private:
    const int id;
    config::Config static_config;
    config::Config dynamic_config;
    std::unique_ptr<tcp::Client> robot_client;
    std::unique_ptr<robot::Clock> clock_client;
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

    double current_time = 0;
    double eta_start_time = 0;
    double hold_until = 0;

    int last_update_time;

    int get_closest_waypoint(std::function<bool(Waypoint)> pred);
};
} // namespace robot
#endif
