#ifndef LIDAR_WRAPPER_HPP
#define LIDAR_WRAPPER_HPP

#include <webots/Lidar.hpp>

struct lidar_wrapper {
    webots::Lidar* lidar;

    int get_number_of_points() const;
    const float* get_range_image() const;
    const webots::LidarPoint *get_point_cloud() const;
    double get_fov() const;
    double get_max_range() const;
    double get_resolution() const;
    void enable(const int time_step);
    bool is_enabled() const;
};

#endif
