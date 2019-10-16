#ifndef __POINT_OPS_HPP
#define __POINT_OPS_HPP

#include <cmath>
#include <iostream>

constexpr double PI = 3.141926535;

struct Point {
    double x, y, z;
};

struct CoordinateSystem {
    Point origin;
    double rotation;

    Point to_global_coordinates(const Point& point) const;

    //Point from_global_coordinates(const Point& point);
};

double euclidean_distance(const Point &p1, const Point &p2);

/* Get angle of line relative to the x-axis. */
double get_angle_of_line(const Point &frontPos, const Point &backPos);

Point operator+(const Point &p1, const Point &p2);

Point operator*(double scale, const Point &p);

std::ostream &operator<<(std::ostream &os, const Point &p);

Point get_average(const Point &p1, const Point &p2);

double get_relative_angle(const Point &origin, const Point &p1, const Point &p2);

#endif
