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

int8_t* is_endstation();
int8_t* station_visited();
int32_t* station_dist();
int32_t* station_schedule();
int32_t* eta();
int32_t* waypoint_dist();
int32_t* waypoint_visited();
int32_t* station_waypoint();
int32_t* waypoint_schedule();

void __ON_CONSTRUCT__();
void __ON_DESTRUCT__();

#ifdef __cplusplus
}
#endif

#endif
