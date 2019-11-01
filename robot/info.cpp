#include "info.hpp"
#include <utility>
#include <vector>

#include <json/json.h>

Json::Value robot::Info::to_json() const
{
    Json::Value json;

    json["id"] = id;
    json["eta"] = eta;
    json["station_plan"] = Json::Value{Json::arrayValue};

    for (std::size_t i = 0; i != station_plan.size(); ++i) {
        json["station_plan"][Json::ArrayIndex(i)] = station_plan[i];
    }

    for (std::size_t i = 0; i != waypoint_plan.size(); ++i) {
        json["waypoint_plan"][Json::ArrayIndex(i)] = waypoint_plan[i];
    }

    return json;
}

robot::Info robot::Info::from_json(const Json::Value &json)
{
    std::pair<double, double> location{json["location"]["x"].asDouble(),
                                       json["location"]["y"].asDouble()};

    std::vector<int> station_plan;

    for (auto itr = json["station_plan"].begin(); itr != json["station_plan"].end(); itr++) {
        station_plan.push_back(itr.key().asInt());
    }

    std::vector<int> waypoint_plan;

    for (auto itr = json["waypoint_plan"].begin(); itr != json["waypoint_plan"].end(); itr++) {
        waypoint_plan.push_back(itr.key().asInt());
    }

    robot::Info info{json["id"].asInt(), location, station_plan, waypoint_plan,
                     json["eta"].asDouble()};

    return info;
}
