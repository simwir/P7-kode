
#ifndef P7_KODE_BROADCASTER_HPP
#define P7_KODE_BROADCASTER_HPP

#include <map>
#include "../../tcp/server.hpp"
#include ""

enum class Functions{get_robot_locations, post_robot_location};

class broadcaster {
public:
    explicit broadcaster(int port);
    void start_broadcasting();

private:
    tcp::Server server;
    robot_data robot_data;
};

Functions parse_function(const std::string& function);

#endif //P7_KODE_BROADCASTER_HPP
