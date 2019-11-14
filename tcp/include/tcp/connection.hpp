/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef TCP_CONNECTION_HPP
#define TCP_CONNECTION_HPP

#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace tcp {

class SendException : public std::exception {
    std::string message;

  public:
    SendException(const std::string &in_message) : message("Could not send message: " + in_message)
    {
    }

    const char *what() const noexcept { return message.c_str(); }
};

class ReceiveException : public std::exception {
    std::string msg;

  public:
    ReceiveException(int err) : msg("Could not read receive buffer: " + std::to_string(err)) {}

    ReceiveException() : msg("Got 0 bytes") {}

    const char *what() const noexcept { return msg.c_str(); }
};

struct MalformedMessageException : public std::exception {
    std::string message;

  public:
    MalformedMessageException(const std::string &in_message)
        : message(std::string("Missing start sequence: " + in_message))
    {
    }

    const char *what() const noexcept { return message.c_str(); }
};

class Connection : public std::enable_shared_from_this<Connection> {
  public:
    Connection(int fd) : fd(fd) { ready = true; }
    Connection() {}
    virtual ~Connection();
    std::optional<std::string> receive_nonblocking();
    std::string receive_blocking();
    ssize_t send(const std::string &message, int flags = 0);
    void close();

    bool closed();

  protected:
    void set_fd(int fd);
    std::optional<std::string> parse_message();

  private:
    std::optional<std::string> receive(bool blocking);
    int fd;
    std::string obuffer;
    bool open = true;
    bool ready;

    friend class Server;
};

} // namespace tcp

#endif // TCP_CONNECTION_HPP
