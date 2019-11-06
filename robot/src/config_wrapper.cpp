#include "robot/config_wrapper.h"
#include "robot/config.hpp"

robot::Config config;

void set_config_path(char *file_path)
{
    config.load_from_file(file_path);
}

CONFIG_GETTER(int, number_of_stations);
CONFIG_GETTER(int, number_of_end_stations);
CONFIG_GETTER(int, number_of_robots);
CONFIG_GETTER(int, number_of_waypoints);
CONFIG_GETTER(int, waypoint_passing_time);
CONFIG_GETTER(int, station_passing_time);
CONFIG_GETTER(int, current_station);
CONFIG_GETTER(int, current_waypoint);
CONFIG_GETTER(int, destination);
CONFIG_GETTER(double, uncertainty);

void station_distance_matrix(int number_of_stations,
                             int matrix[number_of_stations][number_of_stations])
{
}
