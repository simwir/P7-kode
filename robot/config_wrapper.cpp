/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "config_wrapper.hpp"
#include "config.hpp"

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
