
#include "lidar_wrapper.hpp"

int LidarWrapper::get_number_of_points() const
{
    return lidar->getNumberOfPoints();
}
const float *LidarWrapper::get_range_image() const
{
    return lidar->getRangeImage();
}
const webots::LidarPoint *LidarWrapper::get_point_cloud() const
{
    return lidar->getPointCloud();
}
double LidarWrapper::get_fov() const
{
    return lidar->getFov();
}
double LidarWrapper::get_max_range() const
{
    return lidar->getMaxRange();
}
double LidarWrapper::get_min_range() const
{
    return lidar->getMinRange();
}
int LidarWrapper::get_resolution() const
{
    return lidar->getHorizontalResolution();
}
void LidarWrapper::enable(const int time_step)
{
    lidar->enable(time_step);
    // lidar->enablePointCloud();
}
bool LidarWrapper::is_enabled() const
{
    return lidar->getSamplingPeriod() != 0;
}
