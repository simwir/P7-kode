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
#ifndef CONFIG_WRAPPER_HPP
#define CONFIG_WRAPPER_HPP

#define STATIC_CONFIG_GETTER(type, key)                                                            \
    type key() { return static_config.get<type>(#key); }

#ifdef __cplusplus
extern "C" {
#endif

void set_config_path(char *file_path);
int number_of_stations();
int number_of_end_stations();
int number_of_robots();
int number_of_waypoints();
int waypoint_passing_time();
int station_passing_time();
int current_station();
int current_waypoint();
int destination();
double uncertainty();
void station_distance_matrix(int number_of_stations,
                             int matrix[number_of_stations][number_of_stations]);
void waypoint_distance_matrix(int number_of_waypoints,
                              int matrix[number_of_waypoints][number_of_waypoints]);
void other_robot_station_schedules(int number_of_robots, int number_of_stations,
                                   int matrix[number_of_robots][number_of_stations]);
void other_robot_waypoint_schedules(
    int number_of_robots, int number_of_waypoints,
    int matrix[number_of_robots]
              [number_of_waypoints * 2]); // * 2 because we expect to visit each waypoint at most
                                          // once and hold at most once at each waypoint.

#ifdef __cplusplus
}
#endif

#endif
