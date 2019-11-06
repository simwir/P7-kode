#ifndef MASTER_HPP
#define MASTER_HPP

#include <filesystem>

#include <tcp/client.hpp>
#include "config.hpp"
#include "info.hpp"

namespace robot {
class RecievedMessageException : public std::exception {
    std::string message;

  public:
    RecievedMessageException(const std::string &in_message) : message(in_message) {}

    const char *what() const noexcept override { return message.c_str(); }
};

class Master {
  public:
    Master(const std::string &robot_host, const std::string &broadcast_host, int robot_id);
    void load_webots_to_config(std::filesystem::path input_file);
    void request_broadcast_info();
    void send_robot_info(int robot_id, const Info& robot_info);
    std::string recv_broadcast_info();

  private:
    robot::Config config;
    std::unique_ptr<tcp::Client> webot_client;
    tcp::Client broadcast_client;
};
} // namespace robot
#endif
