#include "location.hpp"

#include <json/json.h>

Json::Value robot::Location::to_json() const {
  Json::Value json{Json::objectValue};
  json["x"] = x;
  json["y"] = y;
  return json;
};

robot::Location robot::Location::from_json(const Json::Value &json) {
  return robot::Location{json["x"].asDouble(), json["y"].asDouble()};
}

Json::Value robot::LocationMap::to_json() const {
  Json::Value json{Json::objectValue};

  for (auto &[robot_id, location] : locations) {
    json[std::to_string(robot_id)] = location.to_json();
  }

  return json;
}

robot::Location &robot::LocationMap::operator[](int index) {
  return locations[index];
}

robot::LocationMap robot::LocationMap::from_json(const Json::Value &json) {
  robot::LocationMap location_map;

  for (auto itr = json.begin(); itr != json.end(); itr++) {
    location_map[std::stoi(itr.name())] =
        robot::Location::from_json(json[itr.name()]);
  }

  return location_map;
}
