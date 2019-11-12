#include "config/config_wrapper.hpp"
#include "config/config.hpp"
#include <iostream>

#define STATIC_CONFIG_GETTER(type, key, default)      \
    type key() {                                      \
      try {                                           \
        check_loaded();                               \
        return static_config.get<type>(#key);         \
      }                                               \
      catch (const std::exception& e) {               \
        std::cout << "\n" << e.what() << std::endl;   \
        return default;                               \
      }                                               \
    }

config::Config static_config;
bool static_loaded = false;

void check_loaded()
{
    if (!static_loaded) {
      static_config.load_from_file("static_config.json");
      static_loaded = true;
    }
}

STATIC_CONFIG_GETTER(int, number_of_stations, 0);
STATIC_CONFIG_GETTER(int, number_of_end_stations, 0);
STATIC_CONFIG_GETTER(int, number_of_robots, 0);
STATIC_CONFIG_GETTER(int, number_of_waypoints, 0);
STATIC_CONFIG_GETTER(int, waypoint_passing_time, 0);
STATIC_CONFIG_GETTER(int, station_passing_time, 0);
STATIC_CONFIG_GETTER(int, current_station, 0);
STATIC_CONFIG_GETTER(int, current_waypoint, 0);
STATIC_CONFIG_GETTER(int, destination, 0);
STATIC_CONFIG_GETTER(double, uncertainty, 0.0);

void station_distance_matrix(int number_of_stations,
                             int* matrix)
{
}
