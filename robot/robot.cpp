#include "robot.hpp"
#include "../tcp/utility/split.hpp"
#include <iostream>
#include <sstream>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <fstream>

template <>
std::vector<int> robot::convert_from_json<std::vector<int>>(const Json::Value &value)
{
    std::vector<int> result;

    for (auto itr = value.begin(); itr != value.end(); itr++) {
        result.push_back(value[itr.index()].asInt());
    }

    return result;
}

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

template <>
std::map<int, robot::location> robot::convert_from_json<std::map<int, robot::location>>(const Json::Value &value) {
    std::map<int, robot::location> map;

    for (auto itr = value.begin(); itr != value.end(); itr++){
        map.insert({itr.key().asInt(), robot::location::from_json(value[itr.name()])});
    }
    return map;
}

template <>
std::map<int, robot::plan> robot::convert_from_json<std::map<int, robot::plan>>(const Json::Value &value){
    std::map<int, robot::plan> map;

    for (auto itr = value.begin(); itr != value.end(); itr++){
        map.insert({itr.key().asInt(), robot::plan::from_json(value[itr.name()])});
    }

    return map;
}