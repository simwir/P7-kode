#include "apsp.hpp"

#include "distance_matrix.hpp"
#include <cfloat>
#include <sstream>

// Algorithm from: https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm
apsp_result all_pairs_shortest_path(const AST &ast)
{
    apsp_result result;

    // Initialise dist to DBL_MAX
    // Initialize next to -1
    for (auto &[rowId, _] : ast.nodes) {
        std::map<int, double> column;
        std::map<int, int> next_column;

        for (auto &[colId, _] : ast.nodes) {
            column.insert(std::make_pair(colId, DBL_MAX)); //[colId] = DBL_MAX;
            next_column.insert(std::make_pair(colId, -1)); //[colId] = -1;
        }
        result.dist.insert(std::make_pair(rowId, column));
        result.next.insert(std::make_pair(rowId, next_column));
    }

    // For each edge, set the distance between the vertices to the edge length.
    for (auto &[_, from] : ast.nodes) {
        for (auto adjId : from.adjlist) {
            Waypoint to = ast.nodes.at(adjId);
            result.dist.at(from.id).at(adjId) = euclidean_distance(from, to);
            result.next.at(from.id).at(adjId) = adjId;
        }
    }

    // For each vertex set the distance to yourself to 0
    for (auto &[nodeId, _] : ast.nodes) {
        result.dist.at(nodeId).at(nodeId) = 0.0;
        result.next.at(nodeId).at(nodeId) = nodeId;
    }

    // Calculate all pairs shortest path
    size_t num_waypoints = ast.nodes.size();
    for (size_t k = 0; k < num_waypoints; k++) {
        for (size_t i = 0; i < num_waypoints; i++) {
            for (size_t j = 0; j < num_waypoints; j++) {
                double new_dist = result.dist.at(i).at(k) + result.dist.at(k).at(j);
                if (result.dist.at(i).at(j) > new_dist) {
                    result.dist.at(i).at(j) = new_dist;
                    result.next.at(i).at(j) = result.next.at(i).at(k);
                }
            }
        }
    }

    return result;
}

std::string print_num_waypoints(const std::map<int, std::map<int, double>> &dist)
{
    size_t num_waypoints = dist.size();
    std::stringstream ss;
    ss << "const int NUM_WAYPOINTS = " << num_waypoints << ";\n";
    return ss.str();
}

std::string print_all_pairs_shortest_path_dist(const std::map<int, std::map<int, double>> &dist)
{
    size_t num_waypoints = dist.size();
    std::stringstream ss;
    ss << "const int shortest_path_length[NUM_WAYPOINTS][NUM_WAYPOINTS] = {\n";
    for (size_t i = 0; i < num_waypoints; i++) {
        ss << "  {";
        for (size_t j = 0; j < num_waypoints; j++) {
            ss << dist.at(i).at(j);
            if (j < num_waypoints - 1)
                ss << ",";
        }
        ss << "}";
        if (i < num_waypoints - 1)
            ss << ",\n";
    }
    ss << "\n};\n";
    return ss.str();
}

std::string print_all_pairs_shortest_path_next(const std::map<int, std::map<int, int>> &next)
{
    size_t num_waypoints = next.size();
    std::stringstream ss;
    ss << "const int shortest_path_next[NUM_WAYPOINTS][NUM_WAYPOINTS] = {\n";
    for (size_t i = 0; i < num_waypoints; i++) {
        ss << "  {";
        for (size_t j = 0; j < num_waypoints; j++) {
            ss << next.at(i).at(j);
            if (j < num_waypoints - 1)
                ss << ",";
        }
        ss << "}";
        if (i < num_waypoints - 1)
            ss << ",\n";
    }
    ss << "\n};\n";
    return ss.str();
}
