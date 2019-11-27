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
#ifndef COM_MODULE_HPP
#define COM_MODULE_HPP

#include "communication/info.hpp"
#include "tcp/server.hpp"
#include "util/json.hpp"
#include <map>
#include <memory>
#include <vector>

namespace communication {
enum class Function { get_robot_info, put_robot_info };

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
    void put_robot_info(const std::string &robot_payload);

  private:
    tcp::Server server;

    robot::InfoMap robot_info;
};

} // namespace communication

#endif // COM_MODULE_HPP
