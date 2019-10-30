#ifndef BROADCASTER_ROBOT_HPP
#define BROADCASTER_ROBOT_HPP

#include <map>
#include <vector>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <iostream>
#include <sstream>

namespace robot {

    struct location {
        double x, y;

        Json::Value to_json() const {
            Json::Value value;
            value["x"] = x;
            value["y"] = y;
            return value;
        }
        static location from_json(Json::Value json) {
            return location{json["x"].asDouble(),json["y"].asDouble()};
        }

        std::string toString() {
            return std::to_string(x) + " " + std::to_string(y);
        }
    };

    struct plan {
        int station_id;
        double time;

        Json::Value to_json() const {
            Json::Value value;
            value["station_id"] = station_id;
            value["time"] = time;
            return value;
        }

        static plan from_json(Json::Value json) {
            return plan{json["station_id"].asInt(), json["time"].asDouble()};
        }

        std::string toString() {
            return std::to_string(station_id) + " " + std::to_string(time);
        }
    };

    std::map<int, location> location_map;
    std::map<int, std::vector<plan>> eta_map;
}

#endif //BROADCASTER_ROBOT_HPP
