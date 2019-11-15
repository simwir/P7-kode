#ifndef ROBOT_INFO_HPP
#define ROBOT_INFO_HPP

#include "util/json.hpp"
#include "waypoint_scheduler.hpp"
#include <ostream>
#include <stdexcept>

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace robot {

struct InfoParseError : public std::exception {
    std::string _what;
    InfoParseError(const std::string &msg) : _what("InfoParseError:" + msg) {}

    const char *what() const noexcept override { return _what.c_str(); }
};

class InvalidRobotInfo : public std::exception {
    std::string message;

  public:
    InvalidRobotInfo(const std::string &in_message) : message(in_message) {}
    const char *what() const noexcept override { return message.c_str(); }
};

struct Point {
    double x, y;
};

struct ControllerState {
    Point position;
    bool is_stopped;

    ControllerState() = default;
    ControllerState(double x, double y, bool is_stopped)
        : position(Point{x, y}), is_stopped(is_stopped)
    {
    }
    Json::Value to_json() const;
    static ControllerState from_json(const Json::Value &json);
};
ControllerState parse_controller_state(const std::string &s);

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
