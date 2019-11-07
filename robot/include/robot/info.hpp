#ifndef ROBOT_INFO_HPP
#define ROBOT_INFO_HPP

#include "util/json.hpp"

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace robot {
class Info {
  public:
    Json::Value to_json() const;
    static Info from_json(const std::string &json);
    static Info from_json(const Json::Value &json);

  private:
    int id;
    std::pair<double, double> location;
    std::vector<int> station_plan;
    std::vector<int> waypoint_plan;
    double eta;
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
