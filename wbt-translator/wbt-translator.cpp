#include <fstream>
#include <iostream>
#include <string.h>
#include <vector>

#include "webots_parser.hpp"
#include "distance_matrix.hpp"

using namespace std;

string inputFile;

int main(int argc, char **argv)
{
    if (argc == 1 || argc > 2) {
        std::cout << "Usage: " << argv[0] << " INPUT_FILE\n";
        exit(0);
    }

    inputFile = argv[1];

    ifstream infile{inputFile};
    if (!infile.is_open()) {
        cout << "The file " << inputFile << " could not be opened.\n";
        exit(1);
    }
    AST ast = Parser{infile}.parse_stream();
    distance_matrix dist{ast};
    cout << dist.to_uppaal_declaration();
}
