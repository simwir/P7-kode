

#include "webots_parser.hpp"
#include <iomanip>

std::string Parser::read_token()
{
    std::string token;
    if (!(is >> token)) {
        if (is.eof()) {
          throw EndOfStreamException("The stream ended while trying to read a token.");
        }
    }
    return token;
}

std::string Parser::read_string(){
  std::string line;
  is >> std::quoted(line);
  return line;
}

AST Parser::parse_stream(){
  try {
    while (!is.eof()) {
      std::string token = read_token();
      if (token == "Waypoint" || token == "Station" || token == "Endpoint") {
        Waypoint waypoint = parse_waypoint(token);
        ast.nodes.insert(std::make_pair(waypoint.id, waypoint));
      }
    };
  } catch (EndOfStreamException& e) { }
  return ast;
}

Waypoint Parser::parse_waypoint(const std::string& waypointType){
  Waypoint waypoint;
  if (waypointType == "Waypoint") {
      waypoint.waypointType = WaypointType::eVia;
  } else if (waypointType == "Station") {
      waypoint.waypointType = WaypointType::eStation;
  } else if (waypointType == "Endpoint") {
      waypoint.waypointType = WaypointType::eEndPoint;
  } else {
    throw MalformedWorldFileError("Unexpected waypoint type.");
  }
  bool needId = true, needAdj = true, needTranslation = true;
  try {
    while (needId || needAdj || needTranslation){
      std::string token = read_token();
      if (token == "translation") {
        waypoint.translation = parse_translation();
        needTranslation = false;
      } else if (token == "id") {
        waypoint.id = parse_id();
        needId = false;
      } else if (token == "adjList") {
        waypoint.adjlist = parse_adjlist();
        needAdj = false;
      }
    }
  } catch (EndOfStreamException& e) {
    throw MalformedWorldFileError("Unexpected End of stream. World file malformed?");
  }
  return waypoint;
}

Translation Parser::parse_translation(){
  Translation translation;
  translation.x = std::stod(read_token());
  translation.y = std::stod(read_token());
  translation.z = std::stod(read_token());
  return translation;
}

int Parser::parse_id(){
  return std::stoi(read_token());
}

std::vector<int> Parser::parse_adjlist(){
  std::string adjString = read_string();
  size_t pos = 0;
  std::vector<int> adjlist;
  try {
    while ((pos = adjString.find(",")) != std::string::npos) {
      adjlist.push_back(std::stoi(adjString.substr(0, pos)));
      adjString.erase(0, pos + 1);
    }
    adjlist.push_back(std::stoi(adjString));
  } catch (std::invalid_argument& e){
    throw MalformedWorldFileError("Adjlist is not a list of integers. At: " + adjString);
  }
  return adjlist;
}
