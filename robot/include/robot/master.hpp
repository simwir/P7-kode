#ifndef MASTER_HPP
#define MASTER_HPP

#include "config.hpp"
#include "info.hpp"
#include "wbt-translator/webots_parser.hpp"
#include <tcp/client.hpp>

#include <filesystem>
#include <optional>

namespace robot {
class RecievedMessageException : public std::exception {
    std::string message;

  public:
    RecievedMessageException(const std::string &in_message) : message(in_message) {}

    const char *what() const noexcept override { return message.c_str(); }
};

struct InfoMap {
    static InfoMap from_json(const Json::Value &) { return {}; };
};
struct ControllerInfo {
};

class CannotOpenFileException : public std::exception {
    std::string message;

  public:
    CannotOpenFileException() : message("Cannot open file") {}
    CannotOpenFileException(const std::string &msg) : message(msg) {}

    const char *what() const noexcept override { return message.c_str(); }
};

class Master {
  public:
    Master(const std::string &robot_host, const std::string &broadcast_host, int robot_id,
           std::istream &world_file);
    void load_webots_to_config(const std::filesystem::path &input_file);
    void get_dynamic_state();
    void request_broadcast_info();
    void send_robot_info(int robot_id, const Info &robot_info);
    std::string receive_broadcast_info();

    void request_controller_info();
    std::string receive_controller_info();

    void write_static_config(const std::filesystem::path &path);
    void write_dynamic_config(const std::filesystem::path &path);

    void main_loop();

  private:
    Config static_config;
    Config dynamic_config;
    std::unique_ptr<tcp::Client> webot_client;
    tcp::Client broadcast_client;
    Parser webots_parser;

    robot::InfoMap robot_info;
    robot::ControllerInfo controller_info;
};
} // namespace robot
#endif
