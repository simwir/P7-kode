
#ifndef P7_KODE_BROADCASTER_HPP
#define P7_KODE_BROADCASTER_HPP

#include "server.hpp"

enum class Functions{get_robot_locations};

class broadcaster {
public:
    broadcaster(int port);
    void start_broadcasting();

private:
    tcp::Server server;
};

Functions parse_function(const std::string& function);

#endif //P7_KODE_BROADCASTER_HPP
