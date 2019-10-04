#pragma once

#include "webots_parser.hpp"
#include <string>
#include <vector>
#include <cmath>

class distance_matrix {
public:
    distance_matrix(const AST& ast);

    std::string to_uppaal_declaration() const;

    double& data(size_t i, size_t j) { return _data[i][j]; }

private:
    size_t rows, columns;
    std::vector<std::vector<double>> _data;

    double euclidean_distance(const Waypoint& p1, const Waypoint& p2) {
        auto xdist = p1.translation.x - p2.translation.x;
        auto ydist = p1.translation.y - p2.translation.y;
        return std::sqrt(xdist * xdist + ydist * ydist);
    }
};
