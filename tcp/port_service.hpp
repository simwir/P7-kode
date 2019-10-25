#ifndef TRANSLATOR_PORT_SERVICE_HPP
#define TRANSLATOR_PORT_SERVICE_HPP


#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include "server.hpp"

enum class Function { add_robot, get_robot, remove_robot };

class IdAlreadyDefinedException : public std::exception {
    std::string message;

public:
    IdAlreadyDefinedException(const std::string& in_message) { message = in_message; }
    const char* what() const noexcept override { return message.c_str(); }
};

class port_service {
public:
    port_service(int port);
    void start_server();
    ~port_service();
private:
    tcp::Server server;
    std::map<const int, int> robotMap; // {Robot_id , Port_number}
};

#endif //TRANSLATOR_PORT_SERVICE_HPP
