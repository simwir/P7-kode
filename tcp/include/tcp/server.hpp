/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <memory>
#include <vector>

#include "connection.hpp"

constexpr size_t DEFAULT_BACKLOG = 16;

namespace tcp {
class AcceptException : public std::exception {
    std::string msg;

  public:
    AcceptException(int err) : msg("Could not accept: " + std::to_string(err)) {}

    const char *what() const noexcept { return msg.c_str(); }
};

class BindException : public std::exception {
    std::string msg;

  public:
    BindException(int err) : msg("Could not bind: " + std::to_string(err)) {}

    const char *what() const noexcept { return msg.c_str(); }
};

class ListenException : public std::exception {
    std::string msg;

  public:
    ListenException(int err) : msg("Could not listen: " + std::to_string(err)) {}

    const char *what() const noexcept { return msg.c_str(); }
};

class Server {
  public:
    Server(int port, int backlog = DEFAULT_BACKLOG);
    ~Server();
    std::shared_ptr<tcp::Connection> accept();
    void close();
    int get_port();

  private:
    int socket_fd;
    int port;
    bool open = true;
    std::vector<std::weak_ptr<Connection>> clients;
};
} // namespace tcp

#endif
