#ifndef BROADCASTER_HPP
#define BROADCASTER_HPP

#include <map>
#include <tcp/server.hpp>
#include <robot/info.hpp>
#include <vector>
#include <memory>

namespace broadcaster {
enum class Function {
    get_robot_info, post_robot_location
};

class UnknownFunctionException : public std::exception {
    std::string message;

  public:
    UnknownFunctionException(const std::string &in_message) : message(in_message) {}
    const char *what() const noexcept override { return message.c_str(); }
};

class UnknownParameterException : public std::exception {
    std::string message;

  public:
    UnknownParameterException(const std::string &in_message) : message(in_message) {}
    const char *what() const noexcept override { return message.c_str(); }
};

class Broadcaster {
public:
    Broadcaster(int port) : server(port) {}

    void start_broadcasting();
    void call_function(Function function, std::string& value, std::shared_ptr<tcp::Connection> conn);
    Function parse_function(const std::string &function);
    void parse_message(std::shared_ptr<tcp::Connection> conn);
    void get_robot_info(std::shared_ptr<tcp::Connection> conn);
    void post_robot_location(const std::string& value);

private:
    tcp::Server server;

    robot::InfoMap robot_info;
};

}

#endif //BROADCASTER_HPP
