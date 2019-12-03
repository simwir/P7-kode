/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler
 *Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 *associated documentation files (the "Software"), to deal in the Software without restriction,
 *including without limitation the rights to use, copy, modify, merge, publish, distribute,
 *sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or
 *substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 *NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
 *OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
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
    static ControllerState parse(const std::string &s);
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
    size_t size() const noexcept { return robot_info.size(); }

    bool try_erase(int id);

  private:
    std::map<int, Info> robot_info;
};
} // namespace robot

#endif
