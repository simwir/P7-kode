#include "distance_matrix.hpp"

#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>

distance_matrix::distance_matrix(const AST &ast)
{
    rows = columns = ast.nodes.size();
    // TODO: Handle non consecutive ids
    for (size_t i = 1; i < rows; ++i) {
        _data.push_back(std::vector<double>{});
        for (size_t j = 1; j < columns; ++j) {
            if (!ast.are_connected(i, j)) {
                _data[_data.size() - 1].push_back(-1);
            }
            else {
                _data[_data.size() - 1].push_back(
                    euclidean_distance(ast.nodes.at(i), ast.nodes.at(j)));
            }
        }
    }
}

template <typename TContainer>
std::ostream &write_comma_separated(const TContainer &container, std::ostream &os)
{
    auto b = std::begin(container), e = std::end(container);
    while (b != e) {
        os << *b++;
        if (b != e)
            os << ',';
    }
    return os;
}

std::string distance_matrix::to_uppaal_declaration() const
{
    std::stringstream ss;
    ss << "const int NUM_WAYPOINTS = " << rows << ";\n"
       << "const int dist[NUM_WAYPOINTS][NUM_WAYPOINTS] = {\n";
    auto iter = std::begin(_data);
    while (iter != std::end(_data)) {
        ss << "  {";
        write_comma_separated(*iter++, ss);
        ss << "}";
        if (iter != std::end(_data))
            ss << ",\n";
        //iter++;
    }
    ss << "\n};\n";
    return ss.str();
}

double euclidean_distance(const Waypoint &p1, const Waypoint &p2)
{
    auto xdist = p1.translation.x - p2.translation.x;
    auto ydist = p1.translation.y - p2.translation.y;
    return std::sqrt(xdist * xdist + ydist * ydist);
}
