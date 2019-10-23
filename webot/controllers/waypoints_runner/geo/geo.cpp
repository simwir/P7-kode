#include "geo.hpp"

namespace geo {
Point operator+(const Point &p1, const Point &p2)
{
    return {p1.x + p2.x, p1.y + p2.y};
}
Point operator-(const Point &p1, const Point &p2)
{
    return {p1.x - p2.x, p1.y - p2.y};
}

std::ostream &operator<<(std::ostream &os, const Point &p)
{
    return os << "{" << p.x << ',' << p.y << '}';
}

Point get_average(const Point &p1, const Point &p2)
{
    return {(p1.x + p2.x) / 2, (p1.y + p2.y) / 2};
}

Angle angle_of_line(const Point2D &p1, const Point2D &p2)
{
    return {std::atan2(p2.y - p1.y, p2.x - p1.x) + PI};
}

double euclidean_dist(const Point2D &p1, const Point2D &p2)
{
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
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
    if (a.theta > PI) {
        return {2 * PI - a.theta};
    }
    else
        return {a.theta};
}

std::ostream &operator<<(std::ostream &os, const Angle &a)
{
    return os << a.theta;
}

Point rotate_point(const Point &p, const Angle a)
{
    return {p.x * std::cos(a.theta) - p.y * std::sin(a.theta),
            p.x * std::sin(a.theta) + p.y * std::cos(a.theta)};
}

Point to_global_coordinates(Point rel_orig, Angle rel_angle, Point rel_point)
{
    const auto rotated = rotate_point(rel_point, rel_angle);
    return rotated + rel_orig;
}
} // namespace geo
