#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <vector>
#include <memory>

#include "connection.hpp"

constexpr size_t DEFAULT_BACKLOG = 16;

namespace tcp {
struct AcceptException : std::exception {
  const char* what() const noexcept { return "Could not accept"; }
};

struct BindException : std::exception {
    std::string message;
    std::string const_message= "Cound not bind. Errno: ";
    BindException(const std::string& err) {message = const_message + err;};
    const char* what() const noexcept override { return (message).c_str(); }
};

struct ListenException : std::exception {
  const char* what() const noexcept { return "Could not listen"; }
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
}  // namespace tcp

#endif
