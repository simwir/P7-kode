

#include "webots_parser.hpp"


std::string Parser::read_token()
{
    std::string token;
    if (!(stream >> token)) {
        if (stream.eof()) {
          throw EndOfStreamException();
        }
    }
    return token;
}

std::string Parser::read_string(){
  std::string line = std::getline(stream);
  if (stream.eof())
    throw EndOfStreamException();
  return std::qouted(line);
}

AST Parser::parse_stream(){
  try {
    while (true) {
      std::string token = read_token();
      if (token == "DEF"){
        read_token();
      } else if (token == "Waypoint" || token == "Station" || token == "EndPoint") {
        ast.nodes.push_back(parse_waypoint(token));
      }
    };
  } catch (EndOfStreamException& e) {
    return ast;
  }
}

Waypoint Parser::parse_waypoint(std::string token){
  Waypoint waypoint;
  if (token == "Waypoint") {
    waypoint.waypointType = Waypoint;
  } else if (token == "Station") {
    waypoint.waypointType = Station;
  } else if (token == "EndPoint") {
    waypoint.waypointType = EndPoint;
  } else {
    throw std::exception("Unexpected waypoint type.");
  }
  bool needId = true, needAdj = true, needTranslation = true;
  try {
    while (needId || needAdj || needTranslation){
      token = read_token();
      if (token == "translation") {
        waypoint.translation = parse_translation();
        needTranslation = false;
      } else if (token == "id") {
        waypoint.id = parse_id();
        needId = false;
      } else if (token == "adjList") {
        waypoint.ajdList = parse_ajdList();
        needAdj = false;
      }
    }
  } catch (EndOfStreamException& e) {
    throw std::exception("Unexpected End of stream. World file malformed?");
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

std::vector<int> Parser::parse_ajdList(){
  std::string adjString = read_string();
  size_t pos = 0;
  vector<int> adjList;
  while ((pos = adjString.find(",")) != std::string::npos) {
    adjlist.push_back(std::stoi(adjString.substr(0, pos)));
    adjString.erase(0, pos + 1);
  }
  adjList.push_back(std::stoi(adjString));
  return adjList;
}
