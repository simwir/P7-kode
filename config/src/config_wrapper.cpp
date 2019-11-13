#include "config/config_wrapper.hpp"
#include "config/config.hpp"
#include <iostream>

#define CONFIG_GETTER(type, from, key, default)       \
    type key() {                                      \
      try {                                           \
        return from.get<type>(#key);                  \
      }                                               \
      catch (const std::exception& e) {               \
        std::cout << "\n" << e.what() << std::endl;   \
        return default;                               \
      }                                               \
    }

#define LIST_GETTER(type, key)                        \
    type key() {                                      \
      return _## key.data();                          \
    }

#define INT_CONFIG_LOADER(from, key)                  \
    try {                                             \
      _## key = from.get<std::vector<int>>(#key);     \
    }                                                 \
    catch (const std::exception& e) {                 \
      std::cout << "\n" << e.what() << std::endl;     \
    }

#define BOOL_CONFIG_LOADER(from, key)                 \
    try {                                             \
      std::vector<bool> tmp =                         \
        from.get<std::vector<bool>>(#key);            \
                                                      \
      for (int i = 0; i < tmp.size(); i++)            \
          _## key.push_back(tmp[i]);                  \
    }                                                 \
    catch (const std::exception& e) {                 \
      std::cout << "\n" << e.what() << std::endl;     \
    }

config::Config static_config;
config::Config dynamic_config;

// Station schedule related
std::vector<int8_t> _is_endstation; // Length = number_of_stations
std::vector<int8_t> _station_visited; // Length = number_of_stations
std::vector<int32_t> _station_dist; // Length = number_of_stations ^ 2
std::vector<int32_t> _station_schedule; // Length = (number_of_robots - 1) * number_of_stations
std::vector<int32_t> _eta; // Length = (number_of_robots - 1)

// Waypoint schedule related
std::vector<int32_t> _waypoint_dist; // Length = number_of_waypoints ^ 2
std::vector<int32_t> _waypoint_visited; // Length = number_of_waypoints
std::vector<int32_t> _station_waypoint; // Length = number_of_stations
std::vector<int32_t> _waypoint_schedule; // Length = (number_of_robots - 1) * number_of_waypoints * 4

void __ON_CONSTRUCT__() {
    static_config.load_from_file("static_config.json");
    dynamic_config.load_from_file("dynamic_config.json");

    BOOL_CONFIG_LOADER(static_config, is_endstation);
    INT_CONFIG_LOADER(static_config, station_dist);

    INT_CONFIG_LOADER(static_config, waypoint_dist);
    INT_CONFIG_LOADER(static_config, station_waypoint);

    BOOL_CONFIG_LOADER(dynamic_config, station_visited);
    INT_CONFIG_LOADER(dynamic_config, station_schedule);
    INT_CONFIG_LOADER(dynamic_config, eta);

    INT_CONFIG_LOADER(dynamic_config, waypoint_visited);
    INT_CONFIG_LOADER(dynamic_config, waypoint_schedule);
}

void __ON_DESTRUCT__();

CONFIG_GETTER(int32_t, static_config, number_of_stations, 0);
CONFIG_GETTER(int32_t, static_config, number_of_end_stations, 0);
CONFIG_GETTER(int32_t, static_config, number_of_robots, 0);
CONFIG_GETTER(int32_t, static_config, number_of_waypoints, 0);
CONFIG_GETTER(int32_t, static_config, waypoint_passing_time, 0);
CONFIG_GETTER(int32_t, static_config, station_passing_time, 0);
CONFIG_GETTER(double, static_config, uncertainty, 0.0);

CONFIG_GETTER(int32_t, dynamic_config, current_station, 0);
CONFIG_GETTER(int32_t, dynamic_config, current_waypoint, 0);
CONFIG_GETTER(int32_t, dynamic_config, destination, 0);

LIST_GETTER(int8_t*, is_endstation);
LIST_GETTER(int8_t*, station_visited);
LIST_GETTER(int32_t*, station_dist);
LIST_GETTER(int32_t*, station_schedule);
LIST_GETTER(int32_t*, eta);
LIST_GETTER(int32_t*, waypoint_dist);
LIST_GETTER(int32_t*, waypoint_visited);
LIST_GETTER(int32_t*, station_waypoint);
LIST_GETTER(int32_t*, waypoint_schedule);

void station_distance_matrix(int number_of_stations,
                             int* matrix)
{
}
