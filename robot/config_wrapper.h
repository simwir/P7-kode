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
int current_waypoint();
int destination();
double uncertainty();
void station_distance_matrix(int number_of_stations,
                             int matrix[number_of_stations][number_of_stations]);
void waypoint_distance_matrix(int number_of_waypoints,
                              int matrix[number_of_waypoints][number_of_waypoints]);
void other_robot_station_schedules(int number_of_robots, int number_of_stations,
                                   int matrix[number_of_robots][number_of_stations]);
void other_robot_waypoint_schedules(int number_of_robots, int number_of_waypoints,
                                    int matrix[number_of_robots][number_of_waypoints * 2]);

#ifdef __cplusplus
}
#endif

#endif
