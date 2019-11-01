#include <robot/info.hpp>
#include <string>
#include <utility>
#include <vector>

#include <json/json.h>

namespace robot {
Json::Value Info::to_json() const
{
    Json::Value json;

    json["id"] = id;
    json["eta"] = eta;
    json["station_plan"] = Json::Value{Json::arrayValue};

    for (const int &station : station_plan) {
        json["station_plan"].append(Json::Value{station});
    }

    json["waypoint_plan"] = Json::Value{Json::arrayValue};

    for (const int &waypoint : waypoint_plan) {
        json["waypoint_plan"].append(Json::Value{waypoint});
    }

    return json;
}

Info Info::from_json(std::string &json)
{
    return Info::from_json(Json::Value{json});
}

Info Info::from_json(const Json::Value &json)
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

    Info info{json["id"].asInt(), location, station_plan, waypoint_plan, json["eta"].asDouble()};

    return info;
}
} // namespace robot
