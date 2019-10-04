#include "distance_matrix.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

distance_matrix::distance_matrix(const AST &ast)
{
    rows = columns = ast.nodes.size();

    for (size_t i = 0; i < rows; ++i) {
        _data.push_back(std::vector<double>{});
        for (size_t j = 0; j < columns; ++j) {
            if (!ast.are_connected(i, j)) {
                _data[i].push_back(-1);
            }
            else {
                _data[i].push_back(euclidean_distance(ast.nodes[i], ast.nodes[j]));
            }
        }
    }
}

template <typename TContainer>
std::ostream &write_comma_separated(const TContainer &container, std::ostream &os)
{
    auto b = std::begin(container), e = std::end(container);
    while (b != e - 1) {
        os << *b++ << ',';
    }
    return os << *b;
}

std::string distance_matrix::to_uppaal_declaration() const
{
    std::stringstream ss;
    ss << "const int NUM_WAYPOINTS = " << rows << "\n"
       << "const int dist[NUM_WAYPOINTS][NUM_WAYPOINTS] = {\n";
    for (size_t i = 0; i < rows-1; ++i) {
        ss << "  {"; write_comma_separated(_data[i], ss); ss << "},\n";
    }
    ss << "{"; write_comma_separated(_data[rows - 1], ss); ss << "}\n};\n";
    return ss.str();
}
