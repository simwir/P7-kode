#include "points.hpp"

#include <cmath>
#include <iostream>

double euclidean_distance(const Point &p1, const Point &p2)
{
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.z - p2.z, 2));
}

/* Get angle of line relative to the x-axis. */
double get_angle_of_line(const Point &frontPos, const Point &backPos)
{
    double x0 = frontPos.x, z0 = frontPos.z, x1 = backPos.x, z1 = backPos.z;
    double dx = x0 - x1, dz = z0 - z1;
    return std::atan2(dz, dx);
}

Point operator+(const Point &p1, const Point &p2)
{
    return Point{p1.x + p2.x, p1.y + p2.y, p1.z + p2.z};
}

Point operator*(double scale, const Point &p)
{
    return Point{scale * p.x, scale * p.y, scale * p.z};
}

std::ostream &operator<<(std::ostream &os, const Point &p)
{
    return os << "{x:" << p.x << " y:" << p.y << " z:" << p.z << '}';
}

Point get_average(const Point &p1, const Point &p2)
{
    return 0.5 * (p1 + p2);
}

double get_relative_angle(const Point &origin, const Point &p1, const Point &p2)
{
    double d1 = euclidean_distance(origin, p1), d2 = euclidean_distance(origin, p2),
           opp = euclidean_distance(p1, p2);
    // Law of cosines
    return std::acos((d1 * d1 + d2 * d2 - (opp * opp)) / (2 * d1 * d2));
}

// safely calculate angle difference over radians while handling sketchy overflow
double angle_delta(const double angle1, const double angle2)
{
    return std::atan2(std::sin(angle1 - angle2), std::cos(angle1 - angle2));
}

Point CoordinateSystem::to_global_coordinates(const Point &point) const
{
    return Point{
        point.x * std::cos(-rotation) - point.y * std::sin(-rotation),
        0,
        point.z * std::sin(-rotation) + point.x * std::cos(-rotation)
    } + origin;
}
