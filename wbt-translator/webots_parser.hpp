#ifndef WEBOTS_PARSER_H
#define WEBOTS_PARSER_H

#include <iostream>
#include <string>
#include <vector>

struct Translation {
    double x, y, z;
};

enum WaypointType {Waypoint, Station, EndPoint};

struct Waypoint {
    int id;
  std::vector<int> adjList;
    Translation translation;
    WaypointType waypointType;
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
  Parser(std::istream inputStream){
    stream = inputStream;
  }
  private:
    std::istream stream;
    AST ast;

    std::string read_token();
  std::string read_string();
  Waypoint parse_waypoint(std::string);
  Translation parse_translation();
  int parse_id();
  std::vector<int> parse_ajdList();

};



#endif
