#include "apsp.hpp"

#include <cfloat>
#include "distance_matrix.hpp"

// Algorithm from: https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm

std::map<int, std::map<int, double>> all_pairs_shortest_path(const AST& ast){
    std::map<int, std::map<int, double>> dist;

    // Initialise dist to DBL_MAX
    for(auto rowIt = ast.nodes.begin(); rowIt != ast.nodes.end(); rowIt++){
        std::map<int, double> col{};
        for(auto colIt = ast.nodes.begin(); colIt != ast.nodes.end(); colIt++){
            col[colIt->first] = DBL_MAX;
        }
        dist[rowIt->first] = col;
    }

    // For each edge set the distance between the verts to the edge length.
    for(auto nodeIt = ast.nodes.begin(); nodeIt != ast.nodes.end(); nodeIt++){
        Waypoint from = nodeIt->second;
        for(auto adjIt = from.adjList.begin(); adjIt != from.adjList.end(); adjIt++){
            Waypoint to = ast.nodes.at(*adjIt);
            dist[from.id][to.id] = euclidean_distance(from, to);
        }
    }

    // For each vertex set the distance to yourself to 0
    for(auto nodeIt = ast.nodes.begin(); nodeIt != ast.nodes.end(); nodeIt++){
        dist[nodeIt->first][nodeIt->first] = 0.0;
    }

    // Calculate all pairs shortest path
    size_t num_waypoints = ast.nodes.size();
    for(size_t k = 0; k < num_waypoints; k++){
        for(size_t i = 0; i < num_waypoints; i++){
            for(size_t j = 0; j < num_waypoints; j++){
                double new_dist = dist[i][k] + dist[k][j];
                if (dist[i][j] > new_dist)
                    dist[i][j] = new_dist;
            }
        }
    }

    return dist;
}
