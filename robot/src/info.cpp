#include <map>
#include <robot/info.hpp>
#include <string>
#include <utility>
#include <vector>

#if __APPLE__
#include <json/json.h>
#else
#include <jsoncpp/json/json.h>
#endif

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

Info Info::from_json(const std::string &json)
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

    robot::Info info{json["id"].asInt(), location, station_plan, waypoint_plan,
                     json["eta"].asDouble()};

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
    return InfoMap::from_json(Json::Value{json});
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
