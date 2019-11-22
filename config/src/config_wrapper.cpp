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
#include <cstdlib>
#include <iostream>
#include <limits>

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
            _log << e.what();                                                                      \
            return default;                                                                        \
        }                                                                                          \
    }

Log _log{"libconfig.log"};
bool loaded = false;

config::Config static_config;
config::Config dynamic_config;

void load()
{
    if (loaded) {
        return;
    }

    try {
        static_config.load_from_file("static_config.json");
    }
    catch (const std::exception &e) {
        _log << "Could not load static_config.json";
        _log << e.what();
        std::cout << e.what();
    }

    try {
        dynamic_config.load_from_file("dynamic_config.json");
    }
    catch (const std::exception &e) {
        _log << "Could not load dynamic_config.json";
        _log << e.what();
        std::cout << e.what();
    }

    loaded = true;
    _log << "Loaded files";
}

int32_t number_of_stations()
{
    load();
    try {
        static auto num_stations =
            static_config.getSize("stations") + static_config.getSize("end_stations");
        return num_stations;
    }
    catch (const std::exception &e) {
        _log << "number_of_stations";
        _log << e.what();
        return 0;
    }
}

int32_t number_of_end_stations()
{
    load();
    try {
        static auto num_endstations = static_config.getSize("end_stations");
        return num_endstations;
    }
    catch (const std::exception &e) {
        _log << "number_of_end_stations";
        _log << e.what();
        return 0;
    }
}

int32_t number_of_robots()
{
    load();
    try {
        // We always have at least one robot (that is ourself)
        static auto num_robots = dynamic_config.getSize("robot_info_map") + 1;
        return num_robots;
    }
    catch (const std::exception &e) {
        _log << "number_of_robots";
        _log << e.what();
        return 1; // We always have at least one robot
    }
}

int32_t number_of_waypoints()
{
    load();
    try {
        static auto waypoints = static_config.getSize("stations") +
                                static_config.getSize("end_stations") +
                                static_config.getSize("vias");
        return waypoints;
    }
    catch (const std::exception &e) {
        _log << "number_of_waypoints";
        _log << e.what();
        return 0;
    }
}

int32_t waypoint_passing_time()
{
    load();
    try {
        static auto waypoint_delay = static_config.get<int>("waypoint_delay");
        return waypoint_delay;
    }
    catch (const std::exception &e) {
        _log << "waypoint_passing_time";
        _log << e.what();
        return 0;
    }
}

int32_t station_passing_time()
{
    load();
    try {
        static auto station_delay = static_config.get<int>("station_delay");
        return station_delay;
    }
    catch (const std::exception &e) {
        _log << "station_passing_time";
        _log << e.what();
        return 0;
    }
}

CONFIG_GETTER(double, double, static_config, uncertainty, 0.0);
CONFIG_GETTER(int32_t, int, dynamic_config, next_waypoint, 0);

static std::vector<int> combined_stations()
{
    std::vector<int> endstations = static_config.get<std::vector<int>>("end_stations");
    std::vector<int> stations = static_config.get<std::vector<int>>("stations");

    endstations.insert(endstations.end(), stations.begin(), stations.end());

    return endstations;
}

int32_t convert_to_waypoint_id(int32_t station_id)
{
    load();
    try {
        static auto stations = combined_stations();
        return stations.at(station_id - 1); // Because stations are 1-indexed
    }
    catch (const std::exception &e) {
        _log << "convert_to_waypoint_id";
        _log << e.what();
        return -1; // -1 is not a valid waypoint and thus serves as an error value
    }
}

static int next_station_index()
{
    int station = dynamic_config.get<int>("next_station");
    std::vector<int> stations = combined_stations();
    auto it = std::find(stations.begin(), stations.end(), station);

    if (it != stations.end()) {
        return std::distance(stations.begin(), it) + 1; // 0-indexed -> 1-indexed
    }

    throw config::InvalidValueException{"next_station_index"};
}

int32_t next_station()
{
    load();
    try {
        static auto station_index = next_station_index();
        return station_index;
    }
    catch (const std::exception &e) {
        _log << "next_station";
        _log << e.what();
        return 0; // 0 is not a valid station index, and thus UPPAAL wil complain (as it should for
                  // this case).
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
        _log << "destination";
        _log << e.what();
        return -1; // -1 is not a valid waypoint id, and thus UPPAAL wil complain (as it should for
                   // this case).
    }
}

// Convert from vector<int> (waypoint ids) to vector<bool> that encodes if the station at index i
// has been visited
static std::vector<bool> convert_visited_stations()
{
    auto to_visit = dynamic_config.get<std::vector<int>>("stations_to_visit");
    auto stations = combined_stations();
    std::vector<int> endstations = static_config.get<std::vector<int>>("end_stations");

    std::vector<bool> visited;

    for (const auto &station : stations) {
        if (std::find(endstations.begin(), endstations.end(), station) != endstations.end()) {
            visited.push_back(false);
        }
        else {
            visited.push_back(std::find(to_visit.begin(), to_visit.end(), station) ==
                              to_visit.end());
        }
    }

    return visited;
}

void station_visited(int32_t number_of_stations, int8_t *arr)
{
    load();
    try {
        static auto is_visited = convert_visited_stations();
        for (int i = 0; i < number_of_stations; i++) {
            arr[i] = is_visited.at(i);
        }
    }
    catch (const std::exception &e) {
        _log << "station_visited";
        _log << e.what();
    }
}

int32_t get_station_dist(int32_t from, int32_t to)
{
    load();
    try {
        static auto dist =
            static_config.get<std::vector<std::vector<int>>>("station_distance_matrix");
        return dist.at(from - 1).at(to - 1); // We subtract 1 because stations are 1 indexed.
    }
    catch (const std::exception &e) {
        _log << "get_station_dist";
        _log << e.what();
        return std::numeric_limits<int>::max(); // It is very unlikely that we use this path,
                                                // because
    }
}

// Convert from waypoint ids to station ids
static std::vector<std::vector<int>> convert_robot_next_station()
{
    auto station_plans =
        dynamic_config.get<std::vector<std::vector<int>>>("robot_info_map", "station_plan");
    auto stations = combined_stations();

    std::vector<std::vector<int>> plans;

    for (const auto &station_plan : station_plans) {
        std::vector<int> plan;

        for (const auto &station : station_plan) {
            auto it = std::find(stations.begin(), stations.end(), station);

            if (it == stations.end()) {
                throw config::InvalidValueException{"convert_robot_next_station"};
            }

            // We add 1 because stations are 1 indexed
            auto distance = std::distance(stations.begin(), it) + 1;
            plan.push_back(distance);
        }

        plans.push_back(plan);
    }

    return plans;
}

int32_t next_robot_station(int32_t robot, int32_t step)
{
    load();
    try {
        static auto robot_schedules = convert_robot_next_station();
        // Other robots start their index 2 but vectors start their index at 0.
        // Therefore, we substract 2.
        auto robot_schedule = robot_schedules.at(robot - 2);

        // 0 means that we are done
        return static_cast<int>(robot_schedule.size()) > step ? robot_schedule.at(step) : 0;
    }
    catch (const std::exception &e) {
        _log << "next_robot_station";
        _log << e.what();
        return 0; // 0 means that we are done
    }
}

// Get a list of etas from each robot and add eta for the current robot in front
static std::vector<double> convert_eta()
{
    auto etas = dynamic_config.get<std::vector<double>>("robot_info_map", "station_eta");
    etas.insert(etas.begin(), dynamic_config.get<double>("station_eta"));

    return etas;
}

double eta(int32_t robot)
{
    load();
    try {
        static auto etas = convert_eta();
        // Robots are indexed from 1 but vectors are indexed 0.
        // Therefore, we substract 1.
        return etas.at(robot - 1);
    }
    catch (const std::exception &e) {
        _log << "eta";
        _log << e.what();
        return 0;
    }
}

int32_t get_waypoint_dist(int32_t from, int32_t to)
{
    load();
    try {
        static auto dists =
            static_config.get<std::vector<std::vector<int>>>("waypoint_distance_matrix");
        return dists.at(from).at(to);
    }
    catch (const std::exception &e) {
        _log << "get_waypoint_dist";
        _log << e.what();
        return -1; // -1 impies that there is no edge from -> to.
    }
}

// Convert vector<int> (waypoint ids) to vector<bool> that encodes if the waypoint at index i has
// been visited
static std::vector<bool> convert_visited_waypoints()
{
    auto visited_waypoints = dynamic_config.get<std::vector<int>>("visited_waypoints");
    auto num_waypoints = number_of_waypoints();

    std::vector<bool> visited;

    for (int i = 0; i < num_waypoints; i++) {
        visited.push_back(std::find(visited_waypoints.begin(), visited_waypoints.end(), i) !=
                          visited_waypoints.end());
    }

    return visited;
}

void waypoint_visited(int32_t number_of_waypoints, int8_t *arr)
{
    load();
    try {
        static auto is_visited = convert_visited_waypoints();
        for (int i = 0; i < number_of_waypoints; i++) {
            arr[i] = is_visited.at(i);
        }
    }
    catch (const std::exception &e) {
        _log << "waypoint_visited";
        _log << e.what();
    }
}

void station_list(int32_t number_of_stations, int32_t *arr)
{
    load();
    try {
        static auto stations = combined_stations();
        for (int i = 0; i < number_of_stations; i++) {
            arr[i] = stations.at(i);
        }
    }
    catch (const std::exception &e) {
        _log << "station_list";
        _log << e.what();
    }
}

// Action -> ActionType where an int is used to encode ActionType.
static int32_t convert_to_action_type(std::pair<std::string, int> pair)
{
    if (pair.first.compare("Waypoint") == 0) {
        return WAYPOINT;
    }
    else if (pair.first.compare("Hold") == 0) {
        return HOLD;
    }

    return DONE;
}

static std::vector<std::vector<config::Action>> get_waypoint_plan()
{
    static auto waypoint_plans = dynamic_config.get<std::vector<std::vector<config::Action>>>(
        "robot_info_map", "waypoint_plan");
    return waypoint_plans;
}

int32_t get_next_action_type(int32_t robot, int32_t step)
{
    load();
    try {
        static auto waypoint_plans = get_waypoint_plan();
        // Other robot index start at 2.
        // Therefore, we subtract 2.
        auto robot_schedule = waypoint_plans.at(robot - 2);

        return static_cast<int>(robot_schedule.size()) > step
                   ? convert_to_action_type(robot_schedule.at(step))
                   : DONE;
    }
    catch (const std::exception &e) {
        _log << "get_next_action_type";
        _log << e.what();
        return DONE;
    }
}

int32_t get_next_action_value(int32_t robot, int32_t step)
{
    load();
    try {
        static auto waypoint_plans = get_waypoint_plan();
        // Other robot index start at 2.
        // Therefore, we subtract 2.
        auto robot_schedule = waypoint_plans.at(robot - 2);

        return static_cast<int>(robot_schedule.size()) > step ? robot_schedule.at(step).second : 0;
    }
    catch (const std::exception &e) {
        _log << e.what();
        _log << "get_next_action_value";
        return 0;
    }
}
