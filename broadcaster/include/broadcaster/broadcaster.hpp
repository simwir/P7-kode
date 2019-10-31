
#ifndef P7_KODE_BROADCASTER_HPP
#define P7_KODE_BROADCASTER_HPP

#include <map>
#include <tcp/server.hpp>
#include "../../../robot/robot.hpp"

namespace broadcaster {
enum class Functions {
    get_robot_locations, post_robot_location
};

class Broadcaster {
public:
    Broadcaster(int port);

    void start_broadcasting();
    Functions parse_function(std::shared_ptr<tcp::Connection> conn);

private:
    tcp::Server server;

    std::map<int, robot::location> location_map;
    std::map<int, robot::plan> eta_map;
    std::vector<int> station_plan;
    std::vector<int> waypoint_plan;

    void get_robot_locations(std::shared_ptr<tcp::Connection> conn);
    void post_robot_location(Json::Value value);
};

}

#endif //P7_KODE_BROADCASTER_HPP
