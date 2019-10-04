#include <fstream>
#include <iostream>
#include <string.h>
#include <vector>

#include "webots_parser.hpp"
#include "distance_matrix.hpp"

using namespace std;

string inputFile, outputFile;

int main(int argc, char **argv)
{
    AST ast { { {"foo", {1, 1, 0} }, {"bar", {0, 0, 0}}, {"baz", {1, 2, 0}} } };
    distance_matrix dist{ast};
    cout << dist.to_uppaal_declaration();

    if (argc == 1 || argc > 3) {
        std::cout << "Usage: " << argv[0] << " INPUT_FILE [OUTPUT_FILE]\n";
        exit(0);
    }

    inputFile = argv[1];

    if (argc > 2) {
        outputFile = argv[2];
    }
    else {
        // TODO: Set default output file.
    }

    ifstream infile{inputFile};
    if (!infile.is_open()) {
        cout << "The file " << inputFile << " could not be opened.\n";
        exit(1);
    }
}
