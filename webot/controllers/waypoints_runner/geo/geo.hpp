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

struct Angle {
    double theta;
    Angle(const double angle) : theta(std::fmod(angle + 2 * PI, 2 * PI)) { assert(theta >= 0); }
    Angle() : theta(0) {}
};

enum class PointType { Global, Relative };

template <PointType type>
struct Point2D {
    double x, y;
    Point2D() : x(0), y(0) {}

    Point2D(const double x, const double y) : x(x), y(y) {}

    static Point2D from_polar(const double r, Angle a)
    {
        return Point2D{r * std::cos(a.theta), r * std::sin(a.theta)};
    }
};

using RelPoint = Point2D<PointType::Relative>;
using GlobalPoint = Point2D<PointType::Global>;

GlobalPoint operator+(const GlobalPoint &p1, const GlobalPoint &p2);
GlobalPoint operator-(const GlobalPoint &p1, const GlobalPoint &p2);
std::ostream &operator<<(std::ostream &os, const GlobalPoint &p);
std::ostream &operator<<(std::ostream &os, const RelPoint &p);
GlobalPoint get_average(const GlobalPoint &p1, const GlobalPoint &p2);
Angle angle_of_line(const GlobalPoint &p1, const GlobalPoint &p2);
double euclidean_dist(const GlobalPoint &p1, const GlobalPoint &p2);
double euclidean_dist(const RelPoint &p1, const RelPoint &p2);

Angle operator+(const Angle a1, const Angle a2);
Angle operator-(const Angle a1, const Angle a2);
Angle operator-(const Angle);
std::ostream &operator<<(std::ostream &os, const Angle &a);
Angle abs_angle(const Angle);

RelPoint rotate_point(const RelPoint &p, const Angle a);

GlobalPoint to_global_coordinates(GlobalPoint rel_orig, Angle rel_angle, RelPoint rel_point);

} // namespace geo

#endif