#ifndef DISTANCE_MATRIX_HPP
#define DISTANCE_MATRIX_HPP

#include "wbt-translator/webots_parser.hpp"
#include <cmath>
#include <string>
#include <vector>

class distance_matrix {
  public:
    distance_matrix(const AST &ast);

    std::string to_uppaal_declaration() const;
    std::vector<std::vector<double>> get_data() { return _data; }

    double &data(size_t i, size_t j) { return _data[i][j]; }

  private:
    int rows, columns;
    std::vector<std::vector<double>> _data;
};
double euclidean_distance(const Waypoint &p1, const Waypoint &p2);

#endif
