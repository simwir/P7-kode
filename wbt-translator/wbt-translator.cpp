#include <fstream>
#include <getopt.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <filesystem>

#include "apsp.hpp"
#include "distance_matrix.hpp"
#include "uppaal-printer.hpp"
#include "xml-generator.hpp"

#include "webots_parser.hpp"
void print_help(const char *const execute_location)
{
    std::cerr << "Usage: " << execute_location << " [options] INPUT_FILE [options]" << std::endl
              << "-d --dist-matrix         Emit uppaal waypoint distance matrix" << std::endl
              << "-p --shortest-path       Emit the length of the shortest path between all stations "
                 "and endpoints"
              << std::endl
              << "-r --shortest-route      Emit the shortest route between all stations and endpoints"
              << std::endl
              << "-s --stations            Emit stations ids" << std::endl
              << "-e --endpoints           Emit endpoint ids" << std::endl
              << "-v --vias                Emit vias ids" << std::endl
              << "-n --all-nodes           Same as -sev" << std::endl
              << "-x --xml <template file> Generate UPPAAL XML file\n"
              << "-a --all                 Same as -dprsewn";
}

int main(int argc, char **argv)
{
    const char *const shortOpts = "dprsevnax:";
    const option longOpts[] = {
        {"dist-matrix", no_argument, nullptr, 'd'},    {"shortest-path", no_argument, nullptr, 'p'},
        {"shortest-route", no_argument, nullptr, 'r'}, {"stations", no_argument, nullptr, 's'},
        {"endpoints", no_argument, nullptr, 'e'},      {"vias", no_argument, nullptr, 'v'},
        {"all-nodes", no_argument, nullptr, 'n'},      {"all", no_argument, nullptr, 'a'},
        {"xml", required_argument, nullptr, 'x'}};

    bool d, p, r, optstations, optendpoints, optvias, optxml;
    std::string template_path;

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
        case 'x':
            optxml = true;
            template_path = std::string{optarg};
            break;
        case 'a':
            d = p = r = optstations = optendpoints = optvias = optxml = true;
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
    Parser parser{infile};
    AST ast = parser.parse_stream();

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

    if (optxml) {
        std::ifstream template_file{template_path};
        if(!template_file.is_open()){
            std::cerr << "The file " << template_path << " could not be opened" << std::endl;
            exit(1);
        }
        generate_xml(ast, parser.number_of_robots, std::cout, template_file);
        template_file.close();
    }
}
