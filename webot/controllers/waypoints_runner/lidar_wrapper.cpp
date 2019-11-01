
#include "lidar_wrapper.hpp"

int lidar_wrapper::get_number_of_points() const {
    return lidar->getNumberOfPoints();
}
const float* lidar_wrapper::get_range_image() const {
    return lidar->getRangeImage();
}
const webots::LidarPoint *lidar_wrapper::get_point_cloud() const {
    return lidar->getPointCloud();
}
double lidar_wrapper::get_fov() const {
    return lidar->getFov();
}
double lidar_wrapper::get_max_range() const {
    return lidar->getMaxRange();
}
double lidar_wrapper::get_min_range() const {
    return lidar->getMinRange();
}
int lidar_wrapper::get_resolution() const {
    return lidar->getHorizontalResolution();
}
void lidar_wrapper::enable(const int time_step) {
    lidar->enable(time_step);
    //lidar->enablePointCloud();
}
bool lidar_wrapper::is_enabled() const {
    return lidar->getSamplingPeriod() != 0;
}
