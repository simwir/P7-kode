#include "config_wrapper.h"

#include "config.hpp"

robot::Config config;

void set_config_path(char *file_path)
{
    config.load_from_file(file_path);
}
int number_of_stations()
{
    return config.get<int>("number_of_stations");
}
int number_of_end_stations()
{
    return config.get<int>("number_of_end_stations");
}
int number_of_robots()
{
    return config.get<int>("number_of_robots");
}
int number_of_waypoints()
{
    return config.get<int>("number_of_waypoints");
}
double uncertainty()
{
    return config.get<double>("uncertainty");
}
