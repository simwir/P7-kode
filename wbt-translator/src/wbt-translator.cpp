/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <string.h>
#include <vector>

#include "wbt-translator/apsp.hpp"
#include "wbt-translator/distance_matrix.hpp"
#include "wbt-translator/uppaal-printer.hpp"
#include "wbt-translator/webots_parser.hpp"

void print_help(const char *const execute_location)
{
    std::cerr << "Usage: " << execute_location << " [options] INPUT_FILE [options]" << std::endl
              << "-d --dist-matrix    Emit uppaal waypoint distance matrix" << std::endl
              << "-p --shortest-path  Emit the length of the shortest path between all stations "
                 "and endpoints"
              << std::endl
              << "-r --shortest-route Emit the shortest route between all stations and endpoints"
              << std::endl
              << "-s --stations       Emit stations ids" << std::endl
              << "-e --endpoints      Emit endpoint ids" << std::endl
              << "-v --vias           Emit vias ids" << std::endl
              << "-n --all-nodes      Same as -sev" << std::endl
              << "-a --all            Same as -dprsewn";
}

int main(int argc, char **argv)
{
    const char *const shortOpts = "dprsevna";
    const option longOpts[] = {
        {"dist-matrix", no_argument, nullptr, 'd'},    {"shortest-path", no_argument, nullptr, 'p'},
        {"shortest-route", no_argument, nullptr, 'r'}, {"stations", no_argument, nullptr, 's'},
        {"endpoints", no_argument, nullptr, 'e'},      {"vias", no_argument, nullptr, 'v'},
        {"all-nodes", no_argument, nullptr, 'n'},      {"all", no_argument, nullptr, 'a'}};

    bool d, p, r, optstations, optendpoints, optvias;

    int opt;
    while ((opt = getopt_long(argc, argv, shortOpts, longOpts, nullptr)) != -1) {
        switch (opt) {
        case 'd':
            d = true;
            break;
        case 'p':
            p = true;
            break;
        case 'r':
            r = true;
        case 's':
            optstations = true;
            break;
        case 'e':
            optendpoints = true;
            break;
        case 'v':
            optvias = true;
            break;
        case 'n':
            optstations = optendpoints = optvias = true;
            break;
        }
    }

    if (optind == argc) {
        print_help(argv[0]);
        exit(1);
    }

    std::string inputFile = argv[optind];

    std::ifstream infile{inputFile};
    if (!infile.is_open()) {
        std::cerr << "The file " << inputFile << " could not be opened.\n";
        exit(1);
    }
    AST ast = Parser{infile}.parse_stream();

    if (ast.nodes.size() == 0) {
        std::cerr << "Malformed world file. No waypoints found.";
        exit(1);
    }

    if (d) {
        std::cout << distance_matrix{ast}.to_uppaal_declaration();
    }

    apsp_result shortest_path;
    if (p || r) {
        shortest_path = all_pairs_shortest_path(ast);
        std::cout << print_num_waypoints(shortest_path.dist);
    }

    if (p) {
        std::cout << print_all_pairs_shortest_path_dist(shortest_path.dist);
    }

    if (r) {
        std::cout << print_all_pairs_shortest_path_next(shortest_path.next);
    }

    if (optstations) {
        std::cout << print_waypoints_of_type(ast, WaypointType::eStation);
    }

    if (optendpoints) {
        std::cout << print_waypoints_of_type(ast, WaypointType::eEndPoint);
    }

    if (optvias) {
        std::cout << print_waypoints_of_type(ast, WaypointType::eVia);
    }
}
