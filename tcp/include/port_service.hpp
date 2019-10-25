#ifndef PORT_SERVICE_HPP
#define PORT_SERVICE_HPP

#include <string>
#include <map>
#include "server.hpp"

namespace tcp {

class UnreadableFunctionException : public std::exception {
    std::string message;

 public:
    UnreadableFunctionException(const std::string &in_message) { message = in_message; }
    const char *what() const noexcept override { return message.c_str(); }
};

class InvalidParametersException : public std::exception {
    std::string message;

 public:
    InvalidParametersException(const std::string &in_message)
    {
        message = "Invalid number of arguments:" + in_message;
    }
    const char *what() const noexcept override { return message.c_str(); }
};

enum class Function { add_robot, get_robot, remove_robot };

class IdAlreadyDefinedException : public std::exception {
    std::string message;

public:
    IdAlreadyDefinedException(const std::string& in_message) { message = in_message; }
    const char* what() const noexcept override { return message.c_str(); }
};

class PortService {
public:
    PortService(int port) : server(port) { }
    void start();
    ~PortService();
private:
    tcp::Server server;
    std::map<const int, int> robotMap; // {Robot_id , Port_number}
};

}

#endif // PORT_SERVICE_HPP
