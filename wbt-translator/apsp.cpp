#include "apsp.hpp"

#include <cfloat>
#include <sstream>
#include "distance_matrix.hpp"

// Algorithm from: https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm

std::map<int, std::map<int, double>> all_pairs_shortest_path(const AST& ast){
    std::map<int, std::map<int, double>> dist;

    // Initialise dist to DBL_MAX

    for (auto &[rowId, _] : ast.nodes) {
        std::map<int, double> column;
        for (auto &[colId, _] : ast.nodes) {
            column[colId] = DBL_MAX;
        }
        dist[rowId] = column;
    }

    // For each edge set the distance between the verts to the edge length.
    for (auto &[_, from] : ast.nodes) {
        for (auto adjId : from.adjList) {
            Waypoint to = ast.nodes.at(adjId);
            dist.at(from.id).at(adjId) = euclidean_distance(from, to);
        }
    }

    // For each vertex set the distance to yourself to 0
    for (auto &[nodeId, _] : ast.nodes) {
        dist.at(nodeId).at(nodeId) = 0.0;
    }

    // Calculate all pairs shortest path
    size_t num_waypoints = ast.nodes.size();
    for(size_t k = 1; k <= num_waypoints; k++){
        for(size_t i = 1; i <= num_waypoints; i++){
            for(size_t j = 1; j <= num_waypoints; j++){
                double new_dist = dist[i][k] + dist[k][j];
                if (dist[i][j] > new_dist)
                    dist[i][j] = new_dist;
            }
        }
    }

    return dist;
}

std::string print_all_pairs_shortest_pairs(std::map<int, std::map<int, double>> dist){
    size_t num_waypoints = dist.size();
    std::stringstream ss;
    ss << "const int NUM_WAYPOINTS = " << num_waypoints << "\n"
       << "const int shortest_path_length[NUM_WAYPOINTS][NUM_WAYPOINTS] = {\n";
    for(size_t i = 0; i < num_waypoints; i++){
        ss << "  {";
        for (size_t j = 0; j < num_waypoints; j++){
            ss << dist[i][j];
            if (j < num_waypoints-1)
                ss << ",";
        }
        ss << "}";
        if (i < num_waypoints-1)
            ss << ",\n";
    }
    ss << "\n};\n";
    return ss.str();
}
