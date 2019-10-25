#ifndef APSP_HPP
#define APSP_HPP
#include "webots_parser.hpp"
#include <map>

std::map<int, std::map<int, double>> all_pairs_shortest_path(const AST &ast);
std::string print_all_pairs_shortest_pairs(const std::map<int, std::map<int, double>> &dist);

#endif
