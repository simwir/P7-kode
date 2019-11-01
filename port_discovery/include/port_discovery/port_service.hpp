#ifndef PORT_SERVICE_HPP
#define PORT_SERVICE_HPP

#include <map>
#include <string>
#include <tcp/server.hpp>

namespace port_discovery {

class UnreadableFunctionException : public std::exception {
    std::string message;

  public:
    UnreadableFunctionException(const std::string &in_message) : message(in_message) {}
    const char *what() const noexcept override { return message.c_str(); }
};

class InvalidParametersException : public std::exception {
    std::string message;

  public:
    InvalidParametersException(const std::string &in_message)
        : message("Invalid number of arguments:" + in_message)
    {
    }
    const char *what() const noexcept override { return message.c_str(); }
};

enum class Function { add_robot, get_robot, remove_robot };

class IdAlreadyDefinedException : public std::exception {
    std::string message;

  public:
    IdAlreadyDefinedException(const std::string &in_message) { message = in_message; }
    const char *what() const noexcept override { return message.c_str(); }
};

class PortService {
  public:
    PortService(int port) : server(port) {}
    void start();

  private:
    tcp::Server server;
    std::map<int, int> robot_map; // {Robot_id , Port_number}
};

} // namespace port_discovery

#endif // PORT_SERVICE_HPP
