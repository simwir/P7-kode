/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
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
    json["location"]["x"] = location.x;
    json["location"]["y"] = location.y;
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
T get_field_as(const Json::Value &json, const std::string &field);

template <>
std::vector<int> get_field_as<std::vector<int>>(const Json::Value &json, const std::string &field)
{
    std::vector<int> vector;

    if (!json.isMember(field)) {
        return vector;
    }

    for (auto itr = json[field].begin(); itr != json[field].end(); itr++) {
        vector.push_back(itr.key().asInt());
    }
    return vector;
}

template <>
std::vector<scheduling::Action>
get_field_as<std::vector<scheduling::Action>>(const Json::Value &json, const std::string &field)
{
    std::vector<scheduling::Action> waypoint_plan;

    if (!json.isMember(field)) {
        return waypoint_plan;
    }

    for (auto itr = json[field].begin(); itr != json[field].end(); itr++) {
        auto &arr_value = json[field][itr.key().asInt()];
        std::string type_str = arr_value["type"].asString();
        scheduling::ActionType type =
            type_str == "Hold" ? scheduling::ActionType::Hold : scheduling::ActionType::Waypoint;
        int value = arr_value["value"].asInt();
        waypoint_plan.push_back(scheduling::Action{type, value});
    }
    return waypoint_plan;
}

template <>
robot::Point get_field_as<robot::Point>(const Json::Value &json, const std::string &field)
{
    return robot::Point{json[field]["x"].asDouble(), json[field]["y"].asDouble()};
}

template <>
int get_field_as<int>(const Json::Value &json, const std::string &field)
{
    if (json.isMember(field)) {
        return json[field].asInt();
    }
    else
        throw InvalidRobotInfo("Field not found: " + field);
}

template <>
std::optional<double> get_field_as<std::optional<double>>(const Json::Value &json,
                                                          const std::string &field)
{
    if (json.isMember(field)) {
        return json[field].asDouble();
    }
    else
        return std::nullopt;
}

Info Info::from_json(const Json::Value &json)
{
    if (!json.isMember("location") || !json.isMember("id")) {
        throw InvalidRobotInfo("The json value does not contain id or location");
    }

    int id = get_field_as<int>(json, "id");
    auto location = get_field_as<robot::Point>(json, "location");
    auto station_plan = get_field_as<std::vector<int>>(json, "station_plan");
    auto waypoint_plan = get_field_as<std::vector<scheduling::Action>>(json, "waypoint_plan");
    auto eta = get_field_as<std::optional<double>>(json, "eta");

    return robot::Info{id, location, station_plan, waypoint_plan, eta};
}

InfoMap::InfoMap(const std::vector<Info> &infos)
{
    for (const Info &info : infos) {
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
