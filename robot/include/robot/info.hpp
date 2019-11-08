#ifndef ROBOT_INFO_HPP
#define ROBOT_INFO_HPP

#include <util/json.hpp>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace robot {

struct InfoParseError : public std::exception {
    std::string _what;
    InfoParseError(const std::string &msg) : _what("InfoParseError:" + msg) {}
};

struct ControllerState {
    double x;
    double y;
    bool is_stopped;

    Json::Value to_json() const;
    static ControllerState from_json(const Json::Value &json);
};
ControllerState parse_controller_state(const std::string &s);

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
