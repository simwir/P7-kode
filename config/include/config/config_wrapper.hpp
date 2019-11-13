#ifndef CONFIG_WRAPPER_HPP
#define CONFIG_WRAPPER_HPP

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t number_of_stations();
int32_t number_of_end_stations();
int32_t number_of_robots();
int32_t number_of_waypoints();
int32_t waypoint_passing_time();
int32_t station_passing_time();
int32_t current_station();
int32_t current_waypoint();
int32_t destination();
double uncertainty();

// Station schedule related
void endstation(int32_t num_stations, int8_t* arr); // Length = number_of_stations
void station_visited(int32_t num_stations, int8_t* arr); // Length = number_of_stations
void station_dist(int32_t num_stations, int32_t* arr); // Length = number_of_stations ^ 2
int32_t get_station_dist(int32_t from, int32_t to);
void station_schedule(int32_t num_stations, int32_t num_robots, int32_t* arr); // Length = (number_of_robots - 1) * number_of_stations
void eta(int32_t num_robots, double* arr); // Length = (number_of_robots - 1)

// Waypoint schedule related
void waypoint_dist(int32_t number_of_waypoints, int32_t* arr); // Length = number_of_waypoints ^ 2
void waypoint_visited(int32_t number_of_stations, int8_t* arr); // Length = number_of_stations
void station_waypoint(int32_t number_of_stations, int32_t* arr); // Length = number_of_stations
void waypoint_schedule(int32_t number_of_waypoints, int32_t number_of_robots, int32_t* arr); // Length = (number_of_robots - 1) * number_of_waypoints * 4

#ifdef __cplusplus
}
#endif

#endif
