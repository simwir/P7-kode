#include "config/config_wrapper.hpp"
#include "config/config.hpp"
#include "util/log.hpp"
#include <iostream>
#include <typeinfo>

Log log{"libconfig.log"};

#define CONFIG_GETTER(type, json_type, from, key, default)  \
    type key() {                                            \
      try {                                                 \
        if (!loaded) {                                      \
          load();                                           \
        }                                                   \
        return from.get<json_type>(#key);                   \
      }                                                     \
      catch (const std::exception& e) {                     \
        log << e.what();                                    \
        return default;                                     \
      }                                                     \
    }

#define LIST_GETTER(p_type, list_type, from, key)          \
    void key(int32_t num, p_type arr) {                    \
      if (!loaded) {                                       \
        load();                                            \
      }                                                    \
      try {                                                \
        auto tmp = from.get<std::vector<list_type>>(#key); \
        for (int i = 0; i < num; i++) {                    \
          arr[i] = tmp.at(i);                              \
        }                                                  \
      }                                                    \
      catch (const std::exception& e) {                    \
        log << e.what();                                   \
      }                                                    \
    }

bool loaded = false;

config::Config static_config;
config::Config dynamic_config;

void load() {
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

void endstation(int32_t number_of_stations, int8_t* arr) {
  load();
  try {
    auto tmp = static_config.get<std::vector<bool>>("endstation");
    for (int i = 0; i < number_of_stations; i++) {
      arr[i] = tmp.at(i);
    }
  }
  catch (const std::exception& e) {
    log << e.what();
  }
}

void station_visited(int32_t number_of_stations, int8_t* arr) {
  load();
  try {
    auto tmp = dynamic_config.get<std::vector<bool>>("station_visited");
    for (int i = 0; i < number_of_stations; i++) {
      arr[i] = tmp.at(i);
    }
  }
  catch (const std::exception& e) {
    log << e.what();
  }
}

void station_dist(int32_t number_of_stations, int32_t* arr) {
  load();
  try {
    auto tmp = static_config.get<std::vector<std::vector<int>>>("station_dist");
    for (int i = 0; i < number_of_stations; i++) {
      for (int j = 0; j < number_of_stations; j++) {
        arr[i * number_of_stations + j] = tmp.at(i).at(j);
      }
    }
  }
  catch (const std::exception& e) {
    log << e.what();
  }
}

void station_schedule(int32_t number_of_stations, int32_t number_of_robots, int32_t* arr) {
  load();
  try {
    auto tmp = dynamic_config.get<std::vector<std::vector<int>>>("station_schedule");
    for (int i = 0; i < number_of_robots - 1; i++) {
      for (int j = 0; j < number_of_stations; j++) {
        arr[i * number_of_stations + j] = tmp.at(i).at(j);
      }
    }
  }
  catch (const std::exception& e) {
    log << e.what();
  }
}

void eta(int32_t number_of_robots, double* arr) {
  load();
  try {
    auto tmp = dynamic_config.get<std::vector<double>>("eta");
    for (int i = 0; i < number_of_robots - 1; i++) {
      arr[i] = tmp.at(i);
    }
  }
  catch (const std::exception& e) {
    log << e.what();
  }
}

void waypoint_dist(int32_t number_of_waypoints, int32_t* arr) {
  load();
  try {
    auto tmp = static_config.get<std::vector<std::vector<int>>>("waypoint_dist");
    for (int i = 0; i < number_of_waypoints; i++) {
      for (int j = 0; j < number_of_waypoints; j++) {
        arr[i * number_of_waypoints + j] = tmp.at(i).at(j);
      }
    }
  }
  catch (const std::exception& e) {
    log << e.what();
  }
}

void waypoint_visited(int32_t number_of_waypoints, int8_t* arr) {
  load();
  try {
    auto tmp = dynamic_config.get<std::vector<bool>>("waypoint_visited");
    for (int i = 0; i < number_of_waypoints; i++) {
      arr[i] = tmp.at(i);
    }
  }
  catch (const std::exception& e) {
    log << e.what();
  }
}

void station_waypoint(int32_t number_of_stations, int32_t* arr) {
  load();
  try {
    auto tmp = static_config.get<std::vector<int>>("station_waypoint");
    for (int i = 0; i < number_of_stations; i++) {
      arr[i] = tmp.at(i);
    }
  }
  catch (const std::exception& e) {
    log << e.what();
  }
}

void waypoint_schedule(int32_t number_of_stations, int32_t number_of_robots, int32_t* arr) {
  load();
  try {
    /*auto tmp = dynamic_config.get<std::vector<std::vector<int>>>("station_schedule");
    for (int i = 0; i < num_robots - 1; i++) {
      for (int j = 0; j < number_of_stations; j++) {
        arr[i * number_of_stations + j] = tmp.at(i).at(j);
      }
    }*/
  }
  catch (const std::exception& e) {
    log << e.what();
  }
}
