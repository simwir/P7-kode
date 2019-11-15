#ifndef MASTER_HPP
#define MASTER_HPP

#include <filesystem>

#include "config.hpp"
#include "info.hpp"
#include "wbt-translator/webots_parser.hpp"
#include <tcp/client.hpp>

namespace robot {
class RecievedMessageException : public std::exception {
    std::string message;

  public:
    RecievedMessageException(const std::string &in_message) : message(in_message) {}

    const char *what() const noexcept override { return message.c_str(); }
};

class CannotOpenFileException : public std::exception {
    std::string message;

  public:
    CannotOpenFileException() : message("Cannot open file") {}
    CannotOpenFileException(const std::string &msg) : message(msg) {}

    const char *what() const noexcept override { return message.c_str(); }
};

class Orchestrator {
  public:
    Orchestrator(const std::string &robot_host, const std::string &broadcast_host, int robot_id,
                 std::istream &world_file);
    void load_webots_to_config(const std::filesystem::path &input_file);
    void request_broadcast_info();
    void send_robot_info(int robot_id, const Info &robot_info);
    std::string recv_broadcast_info();

    void write_static_config(const std::filesystem::path &path);
    void write_dynamic_config(const std::filesystem::path &path);

  private:
    void add_waypoint_matrix(const AST &ast, int waypoint_count);
    void add_station_matrix(const AST &ast, int waypoint_count);
    void dump_waypoint_info(const AST &ast);

    Config static_config;
    Config dynamic_config;
    std::unique_ptr<tcp::Client> webot_client;
    tcp::Client broadcast_client;
    Parser webots_parser;
};
} // namespace robot
#endif
