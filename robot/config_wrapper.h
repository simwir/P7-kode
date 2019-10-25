#ifndef CONFIG_WRAPPER_HPP
#define CONFIG_WRAPPER_HPP

#ifdef __cplusplus
extern "C" {
#endif

void set_config_path(char *file_path);
int number_of_stations();
int number_of_end_stations();
int number_of_robots();
int number_of_waypoints();
double uncertainty();

#ifdef __cplusplus
}
#endif

#endif
