#ifndef CONFIG_WRAPPER_HPP
#define CONFIG_WRAPPER_HPP

#define CONFIG_GETTER(type, key)                                                                   \
    type key() { return config.get<type>(#key); }

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
double uncertainty();
void station_distance_matrix(int number_of_stations,
                             int matrix[number_of_stations][number_of_stations]);
void way_point_distance_matrix(int number_of_waypoints,
                               int matrix[number_of_waypoints][number_of_waypoints]);
void other_robot_schedules();

#ifdef __cplusplus
}
#endif

#endif
