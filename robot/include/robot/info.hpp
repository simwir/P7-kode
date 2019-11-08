#ifndef ROBOT_INFO_HPP
#define ROBOT_INFO_HPP

#include "util/json.hpp"
#include "waypoint_scheduler.hpp"

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace robot {

class InvalidRobotInfo : public std::exception {
    std::string message;

  public:
    InvalidRobotInfo(const std::string &in_message) : message(in_message) {}
    const char *what() const noexcept override { return message.c_str(); }
};

struct Point {
    double x, y;
};

struct Info {
    int id;
    Point location;
    std::vector<int> station_plan;
    std::vector<scheduling::Action> waypoint_plan;
    std::optional<double> eta;

    Json::Value to_json() const;
    static Info from_json(const std::string &json);
    static Info from_json(const Json::Value &json);
};

class InfoMap {
  public:
    InfoMap() = default;
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
