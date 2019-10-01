#include <iostream>
#include <vector>
#include <string.h>
#include <fstream>

using namespace std;

struct Translation {
  int x, y ,z;
};

struct Waypoint {
  string name;
  Translation translation;
};

struct AST{
  vector<Waypoint> nodes;
};

class EndOfStreamException

class Parser{
  ifstream stream;
  AST ast;

  string read_token();
public:
  AST parse_stream();
};

string Parser::readToken() {
  string token;
  if (!(token >> stream)) {
    if (stream.eof) {
    
    }
  }
  return token;
}

string inputFile, outputFile;

int main(int argc, char** argv) {
  if (argc == 1 || argc > 3) {
    std::cout << "Usage: " << argv[0] << " INPUT_FILE [OUTPUT_FILE]\n";
    exit(0);
  }

  inputFile = argv[1];

  if (argc > 2) {
    outputFile = argv[2];
  } else {
    //TODO: Set default output file.
  }

  ifstream infile{inputFile};
  if (!infile.is_open()){
    cout << "The file " << infile << " could not be opened.\n";
    exit(1);
  }

}

