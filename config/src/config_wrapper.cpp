#include "config/config_wrapper.hpp"
#include "config/config.hpp"

config::Config static_config;

void set_config_path(char *file_path)
{
    static_config.load_from_file(file_path);
}

STATIC_CONFIG_GETTER(int, number_of_stations);
STATIC_CONFIG_GETTER(int, number_of_end_stations);
STATIC_CONFIG_GETTER(int, number_of_robots);
STATIC_CONFIG_GETTER(int, number_of_waypoints);
STATIC_CONFIG_GETTER(int, waypoint_passing_time);
STATIC_CONFIG_GETTER(int, station_passing_time);
STATIC_CONFIG_GETTER(int, current_station);
STATIC_CONFIG_GETTER(int, current_waypoint);
STATIC_CONFIG_GETTER(int, destination);
STATIC_CONFIG_GETTER(double, uncertainty);

void station_distance_matrix(int number_of_stations,
                             int matrix[number_of_stations][number_of_stations])
{
}
