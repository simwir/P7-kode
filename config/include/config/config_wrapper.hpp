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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DONE 0
#define HOLD 1
#define WAYPOINT 2

int32_t number_of_stations();
int32_t number_of_end_stations();
int32_t number_of_robots();
int32_t number_of_waypoints();
int32_t waypoint_passing_time();
int32_t station_passing_time();
int32_t next_station(); // A station id
int32_t destination(); // A waypoint id
int32_t next_waypoint();
double uncertainty();

// Station schedule related
//void endstation(int32_t num_stations, int8_t *arr);      // Length = number_of_stations
void station_visited(int32_t num_stations, int8_t *arr); // Length = number_of_stations
int32_t get_station_dist(int32_t from, int32_t to);
int32_t next_robot_station(int32_t robot, int32_t step);
double eta(int32_t robot);

// Waypoint schedule related
int32_t get_waypoint_dist(int32_t from, int32_t to);
void waypoint_visited(int32_t number_of_stations, int8_t *arr);  // Length = number_of_stations
void station_list(int32_t number_of_stations, int32_t *arr); // Length = number_of_stations
int32_t get_next_action_type(int32_t robot, int32_t step);
int32_t get_next_action_value(int32_t robot, int32_t step);

#ifdef __cplusplus
}
#endif

#endif
