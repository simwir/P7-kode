#include "eta.hpp"

#include <json/json.h>

Json::Value robot::ETA::to_json() const {
  Json::Value value{Json::objectValue};
  value["station_id"] = station_id;
  value["time"] = time;
  return value;
}

robot::ETA robot::ETA::from_json(const Json::Value& json) {
  return robot::ETA{json["station_id"].asInt(), json["time"].asDouble()};
}

// ETA Map
Json::Value robot::ETAMap::to_json() const {
  Json::Value json{Json::objectValue};

  for (auto& [robot_id, etas] : eta_map) {
    std::string key = std::to_string(robot_id);

    json[key] = Json::Value{Json::arrayValue};

    for (std::size_t i = 0; i < etas.size(); i++) {
      json[key][Json::ArrayIndex(i)] = etas[i].to_json();
    }
  }

  return json;
}

std::vector<robot::ETA>& robot::ETAMap::operator[](int index) {
  return eta_map[index];
}

robot::ETAMap robot::ETAMap::from_json(const Json::Value& json) {
  robot::ETAMap eta_map;

  for (auto itr = json.begin(); itr != json.end(); itr++) {
    std::vector<robot::ETA> etas;

    for (auto eta : json[itr.name()]) {
      etas.push_back(robot::ETA::from_json(eta));
    }

    eta_map[std::stoi(itr.name())] = etas;
  }

  return eta_map;
}
