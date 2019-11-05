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

RelPoint operator+(const RelPoint &p1, const RelPoint &p2)
{
    return {p1.x + p2.x, p1.y + p2.y};
}
RelPoint operator-(const RelPoint &p1, const RelPoint &p2)
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
    const auto res = std::atan2((-p2.y) - (-p1.y), p2.x - p1.x);
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
    const auto rotated = rotate_point(rel_point, rel_angle);
    return GlobalPoint{rotated.x, rotated.y} + rel_orig;
}
} // namespace geo
