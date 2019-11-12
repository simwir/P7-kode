#include "robot/info.hpp"
#include <utility>
#include <vector>

#include <json/json.h>

namespace robot {
robot::ControllerState parse_controller_state(const std::string &s)
{
    std::stringstream ss{s};
    double x, y;
    std::string state;
    char sink;
    ss >> x >> sink;
    if (!ss || sink != ',') {
        throw robot::InfoParseError("could not parse controller state from " + s);
    }
    ss >> y >> sink;
    if (!ss || sink != ',') {
        throw robot::InfoParseError("could not parse controller state from " + s);
    }
    ss >> state;
    if (state == "holding") {
        return robot::ControllerState{x, y, true};
    }
    else if (state == "running") {
        return robot::ControllerState{x, y, false};
    }
    else {
        throw robot::InfoParseError("invalid value for state: " + state);
    }
}
robot::ControllerState robot::ControllerState::from_json(const Json::Value &json)
{
    return robot::ControllerState{
            json["x"].asDouble(),
            json["y"].asDouble(),
            json["stopped"].asBool()
    };
}

Json::Value robot::ControllerState::to_json() const
{
    Json::Value val{Json::objectValue};
    val["x"] = x;
    val["y"] = y;
    val["stopped"] = is_stopped;
    return val;
}

} // namespace robot
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
