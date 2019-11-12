#ifndef EUCLID_HPP
#define EUCLID_HPP

#include <cmath>

template <typename P1, typename P2>
double euclidean_distance(const P1& p1, const P2& p2) {
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

#endif
