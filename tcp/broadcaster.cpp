#include "broadcaster.hpp"
#include "server.hpp"


Functions parse_function(const std::string& function){
    if (function == "get_robot_locations") {
        return Functions::get_robot_locations;
    } else {
        UnreadableFunctionException(function);
    }
}

broadcaster::broadcaster(int port) : server(tcp::Server(port)) {}

void broadcaster::start_broadcasting() {
    std::vector<int> client_fds;
    while(true) {
        client_fds.push_back(server.accept());
    }
}