#ifndef APSP_HPP
#define APSP_HPP
#include <map>
#include "webots_parser.hpp"

struct apsp_result {
    std::map<int, std::map<int, double>> dist;
    std::map<int, std::map<int, int>> next;
};
    
struct apsp_result all_pairs_shortest_path(const AST& ast);
std::string print_num_waypoints(const std::map<int, std::map<int, double>>& dist);
std::string print_all_pairs_shortest_path_dist(const std::map<int, std::map<int, double>>& dist);
std::string print_all_pairs_shortest_path_next(const std::map<int, std::map<int, int>>& next);

#endif
