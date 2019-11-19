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
#include "config/config_wrapper.hpp"
#include "config/config.hpp"
#include "util/log.hpp"

#include <algorithm>
#include <iostream>
#include <limits>

Log log{"libconfig.log"};

#define CONFIG_GETTER(type, json_type, from, key, default)                                         \
    type key()                                                                                     \
    {                                                                                              \
        try {                                                                                      \
            if (!loaded) {                                                                         \
                load();                                                                            \
            }                                                                                      \
            return from.get<json_type>(#key);                                                      \
        }                                                                                          \
        catch (const std::exception &e) {                                                          \
            log << e.what();                                                                       \
            return default;                                                                        \
        }                                                                                          \
    }

bool loaded = false;

config::Config static_config;
config::Config dynamic_config;

void load()
{
    if (loaded) {
        return;
    }

    static_config.load_from_file("static_config.json");
    dynamic_config.load_from_file("dynamic_config.json");

    loaded = true;
    log << "Loaded files";
}

int32_t number_of_stations()
{
    load();
    try {
        static auto tmp = static_config.getSize("stations") + static_config.getSize("end_stations");
        return tmp;
    }
    catch (const std::exception &e) {
        log << "number_of_stations";
        log << e.what();
        return 0;
    }
}

int32_t number_of_end_stations()
{
    load();
    try {
        static auto tmp = static_config.getSize("end_stations");
        return tmp;
    }
    catch (const std::exception &e) {
        log << "number_of_end_stations";
        log << e.what();
        return 0;
    }
}

int32_t number_of_robots()
{
    load();
    try {
        static auto tmp = dynamic_config.getSize("robot_info_map") + 1;
        return tmp;
    }
    catch (const std::exception &e) {
        log << "number_of_end_stations";
        log << e.what();
        return 1;
    }
}

int32_t number_of_waypoints()
{
    load();
    try {
        static auto tmp = static_config.getSize("stations") +
                          static_config.getSize("end_stations") + static_config.getSize("vias");
        return tmp;
    }
    catch (const std::exception &e) {
        log << "number_of_waypoints";
        log << e.what();
        return 0;
    }
}

int32_t waypoint_passing_time()
{
    load();
    try {
        static auto tmp = static_config.get<int>("waypoint_delay");
        return tmp;
    }
    catch (const std::exception &e) {
        log << "waypoint_passing_time";
        log << e.what();
        return 0;
    }
}

int32_t station_passing_time()
{
    load();
    try {
        static auto tmp = static_config.get<int>("station_delay");
        return tmp;
    }
    catch (const std::exception &e) {
        log << "station_passing_time";
        log << e.what();
        return 0;
    }
}

CONFIG_GETTER(double, double, static_config, uncertainty, 0.0);
CONFIG_GETTER(int32_t, int, dynamic_config, next_waypoint, 0);

static int next_station_index()
{
    int station = dynamic_config.get<int>("next_station");
    std::vector<int> station_list = static_config.get<std::vector<int>>("stations");
    std::vector<int>::iterator it = std::find(station_list.begin(), station_list.end(), station);

    if (it != station_list.end()) {
        return number_of_end_stations() + std::distance(station_list.begin(), it);
    }

    std::vector<int> endstation_list = static_config.get<std::vector<int>>("end_stations");
    it = std::find(endstation_list.begin(), endstation_list.end(), station);

    if (it != endstation_list.end()) {
        return std::distance(endstation_list.begin(), it);
    }

    throw config::InvalidValueException{"next_station_index"};
}

int32_t next_station()
{
    load();
    try {
        static auto tmp = next_station_index();
        return tmp;
    }
    catch (const std::exception &e) {
        log << "next_station";
        log << e.what();
        return 0;
    }
}

int32_t destination()
{
    load();
    try {
        static auto tmp = dynamic_config.get<int>("next_station");
        return tmp;
    }
    catch (const std::exception &e) {
        log << "destination";
        log << e.what();
        return 0;
    }
}

/*void endstation(int32_t number_of_stations, int8_t *arr)
{
    load();
    try {
        static auto tmp = static_config.get<std::vector<bool>>("endstation");
        for (int i = 0; i < number_of_stations; i++) {
            arr[i] = tmp.at(i);
        }
    }
    catch (const std::exception &e) {
        log << e.what();
    }
}*/

static std::vector<bool> convert_visited_stations()
{
    auto visited_stations = dynamic_config.get<std::vector<int>>("visited_stations");
    auto endstations = static_config.get<std::vector<int>>("end_stations");
    auto stations = static_config.get<std::vector<int>>("stations");

    endstations.insert(endstations.end(), stations.begin(), stations.end());

    std::vector<bool> visited;

    for (const auto &station : endstations) {
        visited.push_back(std::find(visited_stations.begin(), visited_stations.end(), station) !=
                          visited_stations.end());
    }

    return visited;
}

void station_visited(int32_t number_of_stations, int8_t *arr)
{
    load();
    try {
        static auto tmp = convert_visited_stations();
        for (int i = 0; i < number_of_stations; i++) {
            arr[i] = tmp.at(i);
        }
    }
    catch (const std::exception &e) {
        log << "station_visited";
        log << e.what();
    }
}

int32_t get_station_dist(int32_t from, int32_t to)
{
    load();
    try {
        static auto dist =
            static_config.get<std::vector<std::vector<int>>>("station_distance_matrix");
        return dist.at(from - 1).at(to - 1);
    }
    catch (const std::exception &e) {
        log << "get_station_dist";
        log << e.what();
        return std::numeric_limits<int>::max();
    }
}

static std::vector<std::vector<int>> convert_robot_next_station()
{
    auto station_plans =
        dynamic_config.get<std::vector<std::vector<int>>>("robot_info_map", "station_plan");
    auto endstations = static_config.get<std::vector<int>>("end_stations");
    auto stations = static_config.get<std::vector<int>>("stations");

    endstations.insert(endstations.end(), stations.begin(), stations.end());

    std::vector<std::vector<int>> plans;

    for (const auto &station_plan : station_plans) {
        std::vector<int> plan;

        for (const auto &station : station_plan) {
            auto it = std::find(endstations.begin(), endstations.end(), station);

            if (it == plan.end()) {
                throw config::InvalidValueException{"convert_robot_next_station"};
            }

            plan.push_back(std::distance(endstations.begin(), it));
        }

        plans.push_back(plan);
    }

    return plans;
}

int32_t next_robot_station(int32_t robot, int32_t step)
{
    load();
    try {
        static auto tmp = convert_robot_next_station();
        auto robot_schedule = tmp.at(robot - 2);
        return robot_schedule.size() > step ? robot_schedule.at(step) : 0;
    }
    catch (const std::exception &e) {
        log << "next_robot_station";
        log << e.what();
        return 0;
    }
}

static std::vector<double> convert_eta()
{
    auto tmp = dynamic_config.get<std::vector<double>>("robot_info_map", "eta");
    tmp.insert(tmp.begin(), dynamic_config.get<double>("eta"));

    return tmp;
}

double eta(int32_t robot)
{
    load();
    try {
        static auto tmp = convert_eta();
        return tmp.at(robot - 1);
    }
    catch (const std::exception &e) {
        log << "eta";
        log << e.what();
        return 0;
    }
}

int32_t get_waypoint_dist(int32_t from, int32_t to)
{
    load();
    try {
        static auto tmp =
            static_config.get<std::vector<std::vector<int>>>("waypoint_distance_matrix");
        return tmp.at(from).at(to);
    }
    catch (const std::exception &e) {
        log << "get_waypoint_dist";
        log << e.what();
        return -1;
    }
}

static std::vector<bool> convert_visited_waypoints()
{
    auto visited_waypoints = dynamic_config.get<std::vector<int>>("visited_waypoints");
    auto endstations = static_config.get<std::vector<int>>("end_stations");
    auto stations = static_config.get<std::vector<int>>("stations");
    auto vias = static_config.get<std::vector<int>>("vias");

    endstations.insert(endstations.end(), stations.begin(), stations.end());
    endstations.insert(endstations.end(), vias.begin(), vias.end());

    std::vector<bool> visited;

    for (const auto &waypoint : endstations) {
        visited.push_back(std::find(visited_waypoints.begin(), visited_waypoints.end(), waypoint) !=
                          visited_waypoints.end());
    }

    return visited;
}

void waypoint_visited(int32_t number_of_waypoints, int8_t *arr)
{
    load();
    try {
        static auto tmp = convert_visited_waypoints();
        for (int i = 0; i < number_of_waypoints; i++) {
            arr[i] = tmp.at(i);
        }
    }
    catch (const std::exception &e) {
        log << "waypoint_visited";
        log << e.what();
    }
}

static std::vector<int> get_station_list()
{
    auto stations = static_config.get<std::vector<int>>("stations");
    auto endstations = static_config.get<std::vector<int>>("end_stations");

    endstations.insert(endstations.end(), stations.begin(), stations.end());

    return endstations;
}

void station_list(int32_t number_of_stations, int32_t *arr)
{
    load();
    try {
        static auto tmp = get_station_list();
        for (int i = 0; i < number_of_stations; i++) {
            arr[i] = tmp.at(i);
        }
    }
    catch (const std::exception &e) {
        log << "station_list";
        log << e.what();
    }
}

int32_t convert_to_action(std::pair<std::string, int> pair)
{
    if (pair.first.compare("Waypoint") == 0) {
        return WAYPOINT;
    }
    else if (pair.first.compare("Hold") == 0) {
        return HOLD;
    }

    return DONE;
}

static std::vector<std::vector<std::pair<std::string, int>>> get_waypoint_plan()
{
    static auto tmp = dynamic_config.get<std::vector<std::vector<std::pair<std::string, int>>>>(
        "robot_info_map", "waypoint_plan");
    return tmp;
}

int32_t get_next_action_type(int32_t robot, int32_t step)
{
    load();
    try {
        static auto tmp = get_waypoint_plan();
        auto robot_schedule = tmp.at(robot - 2);

        return robot_schedule.size() > step ? convert_to_action(robot_schedule.at(step)) : DONE;
    }
    catch (const std::exception &e) {
        log << "get_next_action_type";
        log << e.what();
        return DONE;
    }
}

int32_t get_next_action_value(int32_t robot, int32_t step)
{
    load();
    try {
        static auto tmp = get_waypoint_plan();
        auto robot_schedule = tmp.at(robot - 2);

        return robot_schedule.size() > step ? robot_schedule.at(step).second : 0;
    }
    catch (const std::exception &e) {
        log << e.what();
        log << "get_next_action_value";
        return 0;
    }
}
