/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

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
