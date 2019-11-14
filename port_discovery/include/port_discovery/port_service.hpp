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
