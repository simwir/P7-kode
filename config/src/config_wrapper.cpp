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

#define LIST_GETTER(p_type, list_type, from, key)                                                  \
    void key(int32_t num, p_type arr)                                                              \
    {                                                                                              \
        if (!loaded) {                                                                             \
            load();                                                                                \
        }                                                                                          \
        try {                                                                                      \
            auto tmp = from.get<std::vector<list_type>>(#key);                                     \
            for (int i = 0; i < num; i++) {                                                        \
                arr[i] = tmp.at(i);                                                                \
            }                                                                                      \
        }                                                                                          \
        catch (const std::exception &e) {                                                          \
            log << e.what();                                                                       \
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
}

CONFIG_GETTER(int32_t, int, static_config, number_of_stations, 0);
CONFIG_GETTER(int32_t, int, static_config, number_of_end_stations, 0);
CONFIG_GETTER(int32_t, int, static_config, number_of_robots, 0);
CONFIG_GETTER(int32_t, int, static_config, number_of_waypoints, 0);
CONFIG_GETTER(int32_t, int, static_config, waypoint_passing_time, 0);
CONFIG_GETTER(int32_t, int, static_config, station_passing_time, 0);
CONFIG_GETTER(double, double, static_config, uncertainty, 0.0);

CONFIG_GETTER(int32_t, int, dynamic_config, current_station, 0);
CONFIG_GETTER(int32_t, int, dynamic_config, current_waypoint, 0);
CONFIG_GETTER(int32_t, int, dynamic_config, destination, 0);

void endstation(int32_t number_of_stations, int8_t *arr)
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
}

void station_visited(int32_t number_of_stations, int8_t *arr)
{
    load();
    try {
        static auto tmp = dynamic_config.get<std::vector<bool>>("station_visited");
        for (int i = 0; i < number_of_stations; i++) {
            arr[i] = tmp.at(i);
        }
    }
    catch (const std::exception &e) {
        log << e.what();
    }
}

int32_t get_station_dist(int32_t from, int32_t to)
{
    load();
    try {
        static auto dist = static_config.get<std::vector<std::vector<int>>>("station_dist");
        return dist.at(from - 1).at(to - 1);
    }
    catch (const std::exception &e) {
        log << e.what();
        return std::numeric_limits<int>::max();
    }
}

int32_t next_robot_station(int32_t robot, int32_t step)
{
    load();
    try {
        static auto tmp = dynamic_config.get<std::vector<std::vector<int>>>("station_schedule");
        auto robot_schedule = tmp.at(robot - 2);
        return robot_schedule.size() > step ? robot_schedule.at(step) : 0;
    }
    catch (const std::exception &e) {
        log << e.what();
        return 0;
    }
}

double eta(int32_t robot)
{
    load();
    try {
        static auto tmp = dynamic_config.get<std::vector<double>>("eta");
        return tmp.at(robot - 1);
    }
    catch (const std::exception &e) {
        log << e.what();
        return 0;
    }
}

int32_t get_waypoint_dist(int32_t from, int32_t to)
{
    load();
    try {
        static auto tmp = static_config.get<std::vector<std::vector<int>>>("waypoint_dist");
        return tmp.at(from - 1).at(to - 1);
    }
    catch (const std::exception &e) {
        log << e.what();
        return -1;
    }
}

void waypoint_visited(int32_t number_of_waypoints, int8_t *arr)
{
    load();
    try {
        static auto tmp = dynamic_config.get<std::vector<bool>>("waypoint_visited");
        for (int i = 0; i < number_of_waypoints; i++) {
            arr[i] = tmp.at(i);
        }
    }
    catch (const std::exception &e) {
        log << e.what();
    }
}

void station_waypoint_mapping(int32_t number_of_stations, int32_t *arr)
{
    load();
    try {
        static auto tmp = static_config.get<std::vector<int>>("station_waypoint");
        for (int i = 0; i < number_of_stations; i++) {
            arr[i] = tmp.at(i);
        }
    }
    catch (const std::exception &e) {
        log << e.what();
    }
}

int32_t convert_to_action(std::pair<std::string, int> pair) {
  if (pair.first.compare("WAYPOINT") == 0) {
    return WAYPOINT;
  }
  else if (pair.first.compare("HOLD") == 0) {
    return HOLD;
  }

  return DONE;
}

int32_t get_next_action_type(int32_t robot, int32_t step)
{
    load();
    try {
        static auto tmp = dynamic_config.get<std::vector<std::vector<std::pair<std::string, int>>>>("station_schedule");
        auto robot_schedule = tmp.at(robot - 2);

        return robot_schedule.size() > step ? convert_to_action(robot_schedule.at(step)) : DONE;
    }
    catch (const std::exception &e) {
        log << e.what();
        return DONE;
    }
}

int32_t get_next_action_value(int32_t robot, int32_t step)
{
    load();
    try {
        static auto tmp = dynamic_config.get<std::vector<std::vector<std::pair<std::string, int>>>>("station_schedule");
        auto robot_schedule = tmp.at(robot - 2);

        return robot_schedule.size() > step ? robot_schedule.at(step).second : 0;
    }
    catch (const std::exception &e) {
        log << e.what();
        return 0;
    }
}
