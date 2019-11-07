#ifndef ROBOT_INFO_HPP
#define ROBOT_INFO_HPP

#include "util/json.hpp"
#include "waypoint_scheduler.hpp"

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace robot {
struct Info {
    int id;
    std::pair<double, double> location;
    std::vector<int> station_plan;
    std::vector<scheduling::Action> waypoint_plan;
    double eta;

    Json::Value to_json() const;
    static Info from_json(const std::string &json);
    static Info from_json(const Json::Value &json);
};

class InfoMap {
  public:
    InfoMap(){};
    InfoMap(const std::vector<Info> &infos);
    Json::Value to_json() const;
    Info &operator[](int index);
    const Info &operator[](int index) const;
    static InfoMap from_json(const std::string &json);
    static InfoMap from_json(const Json::Value &json);

  private:
    std::map<int, Info> robot_info;
};
} // namespace robot

#endif
