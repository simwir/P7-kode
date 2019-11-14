/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler
 *Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 *associated documentation files (the "Software"), to deal in the Software without restriction,
 *including without limitation the rights to use, copy, modify, merge, publish, distribute,
 *sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or
 *substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 *NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
 *OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "geo.hpp"

namespace geo {
GlobalPoint operator+(const GlobalPoint &p1, const GlobalPoint &p2)
{
    return {p1.x + p2.x, p1.y + p2.y};
}
GlobalPoint operator-(const GlobalPoint &p1, const GlobalPoint &p2)
{
    return {p1.x - p2.x, p1.y - p2.y};
}

std::ostream &operator<<(std::ostream &os, const GlobalPoint &p)
{
    return os << p.x << ',' << p.y;
}
std::ostream &operator<<(std::ostream &os, const RelPoint &p)
{
    return os << p.x << ',' << p.y;
}
GlobalPoint get_midpoint(const GlobalPoint &p1, const GlobalPoint &p2)
{
    return {(p1.x + p2.x) / 2, (p1.y + p2.y) / 2};
}

Angle angle_of_line(const GlobalPoint &p1, const GlobalPoint &p2)
{
    const double res = std::atan2((-p2.y) - (-p1.y), p2.x - p1.x);
    return Angle{res};
}

double euclidean_dist(const GlobalPoint &p1, const GlobalPoint &p2)
{
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

double euclidean_dist(const RelPoint &p1, const RelPoint &p2)
{
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

double constrain_angle(double angle)
{
    angle = std::fmod(angle + PI, 2 * PI);
    if (angle < 0)
        angle += 2 * PI;
    return angle - PI;
}

Angle operator+(const Angle a1, const Angle a2)
{
    return Angle{a1.theta + a2.theta};
}

Angle operator-(const Angle a1, const Angle a2)
{
    return Angle{a1.theta - a2.theta};
}

Angle operator-(const Angle a)
{
    return Angle{-a.theta};
}

Angle abs_angle(const Angle a)
{
    if (a.theta < 0)
        return {-a.theta};
    else
        return {a.theta};
}

std::ostream &operator<<(std::ostream &os, const Angle &a)
{
    return os << a.theta;
}

RelPoint rotate_point(const RelPoint &p, const Angle a)
{
    return RelPoint{p.y * std::sin(a.theta) + p.x * std::cos(a.theta),
                    p.y * std::cos(a.theta) - p.x * std::sin(a.theta)};
}

GlobalPoint to_global_coordinates(GlobalPoint rel_orig, Angle rel_angle, RelPoint rel_point)
{
    const RelPoint rotated = rotate_point(rel_point, rel_angle);
    return GlobalPoint{rotated.x, rotated.y} + rel_orig;
}
} // namespace geo
