#ifndef CONFIG_WRAPPER_HPP
#define CONFIG_WRAPPER_HPP

#ifdef __cplusplus
extern "C" {
#endif

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
                             int* matrix);
void waypoint_distance_matrix(int number_of_waypoints,
                              int* matrix);
void other_robot_station_schedules(int number_of_robots, int number_of_stations,
                                   int* matrix);
void other_robot_waypoint_schedules(
    int number_of_robots, int number_of_waypoints,
    int* matrix); // * 2 because we expect to visit each waypoint at most
                                          // once and hold at most once at each waypoint.

void __ON_CONSTRUCT__();
void __ON_DESTRUCT__();

#ifdef __cplusplus
}
#endif

#endif
