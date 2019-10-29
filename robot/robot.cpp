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
        itr.key();
        std::cout << json.get(itr.index(), json);
    }
    return map;
}

int main(int argc, char**){
    //Locations
    struct robot::location loc1{1, 1.1};
    struct robot::location loc2{2, 2.2};

    //Plan
    struct robot::plan plan1{1, 1.1};
    struct robot::plan plan2{2, 2.01};

    //Plans
    std::vector<robot::plan> plans2;
    plans2.push_back(plan1);
    plans2.push_back(plan2);

    //location_map
    std::map<int, robot::location> location_map = {{1337, loc1}};
    location_map.insert({ 2, loc2});

    //eta_map
    std::map<int, std::vector<robot::plan>> eta_map = {{1, plans2}};

    //location_map_json
    Json::Value location_map_json;
    location_map_json = parse_location_map(location_map);
    std::cout << location_map_json;
    parse_json_location_map(location_map_json);
}