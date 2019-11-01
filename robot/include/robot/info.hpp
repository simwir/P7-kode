#ifndef ROBOT_INFO_HPP
#define ROBOT_INFO_HPP

#include <json/json.h>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace robot {
struct Info {
    int id;
    std::pair<double, double> location;
    std::vector<int> station_plan;
    std::vector<int> waypoint_plan;
    double eta;

    Json::Value to_json() const;
    static Info from_json(std::string &json);
    static Info from_json(const Json::Value &json);
};

struct InfoMap {
    std::map<int, Info> info_map;

    InfoMap(std::vector<Info> infos);
    Json::Value to_json() const;
    void insert(Info info);
    void insert(int, Info);
    Info &operator[](int index);
    static InfoMap from_json(const std::string &json);
    static InfoMap from_json(const Json::Value &json);
};
} // namespace robot

#endif
