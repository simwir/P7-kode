#include "robot.hpp"
#include "../tcp/utility/split.hpp"
#include <iostream>
#include <sstream>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <fstream>

Json::Value parse_location_map(const std::map<int, robot::location>& location_map){
    Json::Value location_map_json;

    for (auto& [k , v] : location_map){
        location_map_json[std::to_string(k)] = v.to_json();
    }
    return location_map_json;
}

Json::Value parse_eta_map(const std::map<int, std::vector<robot::plan>>& eta_map){
    Json::Value eta_map_json;

    for (auto& [k , v] : eta_map){
        Json::Value plans;
        for (robot::plan plan : v){
            plans.append(plan.to_json());
        }
        eta_map_json[std::to_string(k)] = plans;
    }
    return eta_map_json;
}

std::map<int, robot::location> parse_json_location_map(Json::Value json){
    std::map<int, robot::location> map;

    for (Json::ValueConstIterator itr = json.begin(); itr != json.end(); itr++){
        robot::location location = {json[itr.name()]["x"].asDouble(), json[itr.name()]["y"].asDouble()};
        map.insert({itr.key().asInt(), location});
    }
    return map;
}

std::map<int, std::vector<robot::plan>> parse_json_eta_map(Json::Value json){
    std::map<int, std::vector<robot::plan>> map;

    for (Json::ValueConstIterator itr = json.begin(); itr != json.end(); itr++){
        std::vector<robot::plan> plans;
        Json::Value entry = json[itr.name()];
        for (auto itr2 = entry.begin(); itr2 != entry.end(); itr2++){
            plans.push_back(robot::plan::from_json(entry[itr2.index()]));
        }
        map.insert({atoi(itr.key().asCString()), plans});
    }
    return map;
}