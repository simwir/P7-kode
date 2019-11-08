#ifndef BROADCASTER_HPP
#define BROADCASTER_HPP

#include "robot/info.hpp"
#include "tcp/server.hpp"
#include "util/json.hpp"
#include <map>
#include <memory>
#include <vector>

namespace communication {
enum class Function { get_robot_info, post_robot_info };

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

class ComModule {
  public:
    ComModule(int port) : server(port) {}

    void start_broadcasting();
    void call_function(Function function, const std::string &parameters,
                       std::shared_ptr<tcp::Connection> conn);
    Function parse_function(const std::string &function);
    void parse_message(std::shared_ptr<tcp::Connection> conn);
    void get_robot_info(std::shared_ptr<tcp::Connection> conn);
    void post_robot_info(const std::string &robot_payload);

  private:
    tcp::Server server;

    robot::InfoMap robot_info;
};

} // namespace broadcaster

#endif // BROADCASTER_HPP
