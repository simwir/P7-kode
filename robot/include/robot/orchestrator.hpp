/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler
 *Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 *associated documentation files (the "Software"), to deal in the Software without restriction,
 *including without limitation the rights to use, copy, modify, merge, publish, distribute,
 *sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or
 *substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 *NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
 *OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef ORCHESTRATOR_HPP
#define ORCHESTRATOR_HPP

#include <filesystem>

#include "config/config.hpp"
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

    config::Config static_config;
    config::Config dynamic_config;
    std::unique_ptr<tcp::Client> webot_client;
    tcp::Client broadcast_client;
    Parser webots_parser;
};
} // namespace robot
#endif
