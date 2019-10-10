#include <fstream>
#include <iostream>
#include <string.h>
#include <vector>

#include "webots_parser.hpp"
#include "distance_matrix.hpp"

int main(int argc, char **argv)
{
    std::string inputFile;
    if (argc == 1 || argc > 2) {
        std::cout << "Usage: " << argv[0] << " INPUT_FILE\n";
        exit(0);
    }

    inputFile = argv[1];

    std::ifstream infile{inputFile};
    if (!infile.is_open()) {
        std::cout << "The file " << inputFile << " could not be opened.\n";
        exit(1);
    }
    AST ast = Parser{infile}.parse_stream();
    distance_matrix dist{ast};
    std::cout << dist.to_uppaal_declaration();
}
