#include "robot/info.hpp"
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace robot {
Json::Value Info::to_json() const
{
    Json::Value json;

    json["id"] = id;
    json["eta"] = eta.has_value() ? eta.value() : Json::nullValue;
    json["location"]["x"] = location.first;
    json["location"]["y"] = location.second;
    json["station_plan"] = Json::Value{Json::arrayValue};

    for (const int &station : station_plan) {
        json["station_plan"].append(Json::Value{station});
    }

    json["waypoint_plan"] = Json::Value{Json::arrayValue};

    for (const scheduling::Action &waypoint : waypoint_plan) {
        json["waypoint_plan"].append(waypoint.to_json());
    }

    return json;
}

Info Info::from_json(const std::string &json)
{
    std::stringstream ss(json);
    Json::Value root;
    ss >> root;

    return Info::from_json(root);
}

template <typename T>
T get_field_as(const Json::Value& json, const std::string &field);

template <>
std::vector<int> get_field_as<std::vector<int>>(const Json::Value& json, const std::string &field) {
    std::vector<int> vector;

    if (!json.isMember(field)){
        return vector;
    }

    for (auto itr = json[field].begin(); itr != json[field].end(); itr++) {
        vector.push_back(itr.key().asInt());
    }
    return vector;
}

template <>
std::vector<scheduling::Action> get_field_as<std::vector<scheduling::Action>>(const Json::Value& json, const std::string &field) {
    std::vector<scheduling::Action> waypoint_plan;

    if (!json.isMember(field)) {
        return waypoint_plan;
    }

    for (auto itr = json[field].begin(); itr != json[field].end(); itr++) {
        std::string type_str = json[field][itr.key().asInt()]["type"].asString();
        scheduling::ActionType type = type_str == "Hold" ? scheduling::ActionType::Hold : scheduling::ActionType::Waypoint;
        int value = json[field][itr.key().asInt()]["value"].asInt();
        waypoint_plan.push_back(scheduling::Action{type, value});
    }
    return waypoint_plan;
}

template <>
std::pair<double, double> get_field_as<std::pair<double, double>>(const Json::Value& json, const std::string &field) {
    std::pair<double, double> location{json["location"]["x"].asDouble(), 
                                       json["location"]["y"].asDouble()};
    return location;
}

template <>
int get_field_as<int>(const Json::Value& json, const std::string &field) {
    if (json.isMember(field)) {
        return json[field].asInt();
    } else throw new InvalidRobotInfo("Robot must know its own id");
}

template <>
std::optional<double> get_field_as<std::optional<double>>(const Json::Value& json, const std::string &field) {
    if (json.isMember(field)) {
        return json[field].asDouble();
    } else return std::nullopt;
}

Info Info::from_json(const Json::Value &json)
{
    if (!json.isMember("location") || !json.isMember("id")) {
        throw new InvalidRobotInfo("The json value does not contain id or location");
    }

    int id = get_field_as<int>(json, "id");
    std::pair<double, double> location = get_field_as<std::pair<double, double>>(json, "location");
    std::vector<int> station_plan = get_field_as<std::vector<int>>(json, "station_plan");
    std::vector<scheduling::Action> waypoint_plan = get_field_as<std::vector<scheduling::Action>>(json, "waypoint_plan");
    std::optional<double> eta = get_field_as<std::optional<double>>(json, "eta");

    robot::Info info{id, location, station_plan, waypoint_plan, eta};

    return info;
}

InfoMap::InfoMap(const std::vector<Info> &infos)
{
    for (Info info : infos) {
        (*this)[info.id] = info;
    }
}

Info &InfoMap::operator[](int index)
{
    return robot_info[index];
}

const Info &InfoMap::operator[](int index) const
{
    return robot_info.at(index);
}

Json::Value InfoMap::to_json() const
{
    Json::Value json{Json::objectValue};

    for (auto &[robot_id, info] : robot_info) {
        json[std::to_string(robot_id)] = info.to_json();
    }

    return json;
}

InfoMap InfoMap::from_json(const std::string &json)
{
    Json::Value root;
    std::stringstream str(json);
    str >> root;
    return InfoMap::from_json(root);
}

InfoMap InfoMap::from_json(const Json::Value &json)
{
    std::vector<Info> infos;

    for (auto &info : json) {
        infos.push_back(Info::from_json(info));
    }

    return InfoMap{infos};
}
} // namespace robot
