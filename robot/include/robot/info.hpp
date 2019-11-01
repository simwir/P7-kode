#ifndef ROBOT_INFO_HPP
#define ROBOT_INFO_HPP

#include <json/json.h>
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
    static Info from_json(const Json::Value &json);
};
} // namespace robot

#endif
