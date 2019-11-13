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
      return _## key;                                 \
    }

#define CONFIG_LOADER(type, from, key)              \
    try {                                           \
      _## key = from.get<type>(#key);               \
    }                                               \
    catch (const std::exception& e) {               \
      std::cout << "\n" << e.what() << std::endl;   \
    }

config::Config static_config;
config::Config dynamic_config;

// Station schedule related
std::vector<bool> _is_endstation; // Length = number_of_stations
std::vector<bool> _station_visited; // Length = number_of_stations
std::vector<int> _station_dist; // Length = number_of_stations ^ 2
std::vector<int> _station_schedule; // Length = (number_of_robots - 1) * number_of_stations
std::vector<int> _eta; // Length = (number_of_robots - 1)

// Waypoint schedule related
std::vector<int> _waypoint_dist; // Length = number_of_waypoints ^ 2
std::vector<int> _waypoint_visited; // Length = number_of_waypoints
std::vector<int> _station_waypoint; // Length = number_of_stations
std::vector<int> _waypoint_schedule; // Length = (number_of_robots - 1) * number_of_waypoints * 4

void __ON_CONSTRUCT__() {
    static_config.load_from_file("static_config.json");
    dynamic_config.load_from_file("dynamic_config.json");

    CONFIG_LOADER(std::vector<bool>, static_config, is_endstation);
    CONFIG_LOADER(std::vector<int>, static_config, station_dist);

    CONFIG_LOADER(std::vector<int>, static_config, waypoint_dist);
    CONFIG_LOADER(std::vector<int>, static_config, station_waypoint);

    CONFIG_LOADER(std::vector<bool>, dynamic_config, station_visited);
    CONFIG_LOADER(std::vector<int>, dynamic_config, station_schedule);
    CONFIG_LOADER(std::vector<int>, dynamic_config, eta);

    CONFIG_LOADER(std::vector<int>, dynamic_config, waypoint_visited);
    CONFIG_LOADER(std::vector<int>, dynamic_config, waypoint_schedule);
}

void __ON_DESTRUCT__();

CONFIG_GETTER(int, static_config, number_of_stations, 0);
CONFIG_GETTER(int, static_config, number_of_end_stations, 0);
CONFIG_GETTER(int, static_config, number_of_robots, 0);
CONFIG_GETTER(int, static_config, number_of_waypoints, 0);
CONFIG_GETTER(int, static_config, waypoint_passing_time, 0);
CONFIG_GETTER(int, static_config, station_passing_time, 0);
CONFIG_GETTER(double, static_config, uncertainty, 0.0);

CONFIG_GETTER(int, dynamic_config, current_station, 0);
CONFIG_GETTER(int, dynamic_config, current_waypoint, 0);
CONFIG_GETTER(int, dynamic_config, destination, 0);

LIST_GETTER(std::vector<bool>, is_endstation);
LIST_GETTER(std::vector<bool>, station_visited);
LIST_GETTER(std::vector<int>, station_dist);
LIST_GETTER(std::vector<int>, station_schedule);
LIST_GETTER(std::vector<int>, eta);
LIST_GETTER(std::vector<int>, waypoint_dist);
LIST_GETTER(std::vector<int>, waypoint_visited);
LIST_GETTER(std::vector<int>, station_waypoint);
LIST_GETTER(std::vector<int>, waypoint_schedule);

void station_distance_matrix(int number_of_stations,
                             int* matrix)
{
}
