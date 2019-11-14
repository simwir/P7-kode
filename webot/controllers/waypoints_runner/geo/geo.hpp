/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
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

double constrain_angle(double angle);

struct Angle {
    double theta;
    Angle(const double angle) : theta(constrain_angle(angle)) {}
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
        const auto angle = a.theta;
        const auto x = r * std::cos(angle);
        const auto y = -r * std::sin(angle);
        return Point2D{x, y};
    }
};

using RelPoint = Point2D<PointType::Relative>;
using GlobalPoint = Point2D<PointType::Global>;

GlobalPoint operator+(const GlobalPoint &p1, const GlobalPoint &p2);
GlobalPoint operator-(const GlobalPoint &p1, const GlobalPoint &p2);
std::ostream &operator<<(std::ostream &os, const GlobalPoint &p);
std::ostream &operator<<(std::ostream &os, const RelPoint &p);
GlobalPoint get_midpoint(const GlobalPoint &p1, const GlobalPoint &p2);
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
