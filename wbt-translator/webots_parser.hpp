#ifndef WEBOTS_PARSER_H
#define WEBOTS_PARSER_H

#include <iostream>
#include <string>
#include <vector>

struct Translation {
    int x, y, z;
};

struct Waypoint {
    std::string name;
    Translation translation;
};

struct AST {
    std::vector<Waypoint> nodes;

    bool are_connected(size_t i, size_t j) const {
        // TODO implement edge logic
        return i != j;
    }
};

class EndOfStreamException;

class Parser {
  public:
    AST parse_stream();

  private:
    std::istream stream;
    AST ast;

    std::string read_token();

};



#endif
