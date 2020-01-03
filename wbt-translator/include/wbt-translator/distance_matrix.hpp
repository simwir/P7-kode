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
#ifndef DISTANCE_MATRIX_HPP
#define DISTANCE_MATRIX_HPP

#include "wbt-translator/webots_parser.hpp"
#include <cmath>
#include <string>
#include <vector>

// the measured speed of the webots robot in metres per second.
constexpr double MEASURED_ROBOT_SPEED = 0.027;

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
