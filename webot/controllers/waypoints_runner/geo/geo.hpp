#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <cassert>
#include <cmath>
#include <iostream>

#ifdef M_PI
constexpr double PI = M_PI;
#else
constexpr double PI = 3.1415926535;
#endif

namespace geo {
struct Point2D {
    double x, y;
    Point2D() : x(0), y(0) {}

    Point2D(const double x, const double y) : x(x), y(y) {}
};
struct Angle {
    double theta;
    Angle(const double angle) : theta(std::fmod(angle + 2 * PI, 2 * PI)) { assert(theta >= 0); }
    Angle() : theta(0) {}
};
using Point = Point2D;

Point operator+(const Point &p1, const Point &p2);
std::ostream &operator<<(std::ostream &os, const Point &p);
Point get_average(const Point &p1, const Point &p2);
Angle angle_of_line(const Point2D &p1, const Point2D &p2);
double euclidean_dist(const Point2D &p1, const Point2D &p2);

Angle operator+(Angle a1, Angle a2);
Angle operator-(Angle a1, Angle a2);
Angle operator-(Angle);

Point rotate_point(const Point &p, const Angle a);
Point to_global_coordinates(Point rel_orig, Angle rel_angle, Point rel_point);

} // namespace geo

#endif
