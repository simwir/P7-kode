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
    int err;
    AcceptException(int err) : msg("Could not accept: " + std::to_string(err)), err(err) {}

    const char *what() const noexcept { return msg.c_str(); }
};

class BindException : public std::exception {
    std::string msg;

  public:
    int err;
    BindException(int err) : msg("Could not bind: " + std::to_string(err)), err(err) {}

    const char *what() const noexcept { return msg.c_str(); }
};

class ListenException : public std::exception {
    std::string msg;

  public:
    int err;
    ListenException(int err) : msg("Could not listen: " + std::to_string(err)), err(err) {}

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
