#include <fstream>
#include <iostream>
#include <string.h>
#include <vector>
#include <getopt.h>

#include "webots_parser.hpp"
#include "distance_matrix.hpp"
#include "apsp.hpp"

void print_help(const char* const execute_location)
{
    std::cerr << "Usage: " << execute_location << " [options] INPUT_FILE [options]" << std::endl
              << "-d --dist-matrix    Emit uppaal waypoint distance matrix" << std::endl
              << "-p --shortest-path  Emit the length of the shortest path between all stations and endpoints" << std::endl
              << "-r --shortest-route Emit the shortest route between all stations and endpoints" << std::endl
              << "-s --stations       Emit stations ids" << std::endl
              << "-e --endpoints      Emit endpoint ids" << std::endl
              << "-w --waypoints      Emit waypoint ids" << std::endl
              << "-n --all-nodes      Emit all node ids" << std::endl
              << "-a --all            Same as -dprsewn";
}

int main(int argc, char **argv)
{
    const char* const shortOpts = "dprsewna";
    const option longOpts[] =
    {
     {"dist-matrix", no_argument, nullptr, 'd'},
     {"shortest-path", no_argument, nullptr, 'p'},
     {"shortest-route", no_argument, nullptr, 'r'},
     {"stations", no_argument, nullptr, 's'},
     {"endpoints", no_argument, nullptr, 'e'},
     {"waypoints", no_argument, nullptr, 'w'},
     {"all-nodes", no_argument, nullptr, 'n'},
     {"all", no_argument, nullptr, 'a'}
    };

    bool d, p, r;

    int opt;
    while ((opt = getopt_long(argc, argv, shortOpts, longOpts, nullptr)) != -1){
        switch(opt){
        case 'd':
            d = true;
            break;
        case 'p':
            p = true;
        }
    }

    if (optind == argc){
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

    if (ast.nodes.size() == 0){
        std::cerr << "Malformed world file. No waypoints found.";
        exit(1);
    }

    if (d) {
        std::cout << distance_matrix{ast}.to_uppaal_declaration();
    }

    std::map<int, std::map<int, double>> shortest_path;
    if (p || r) {
        shortest_path = all_pairs_shortest_path(ast);
    }

    if (p) {
        std::cout << print_all_pairs_shortest_pairs(shortest_path);
    }
}
