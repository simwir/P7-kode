#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <vector>

#include "connection.hpp"

constexpr size_t DEFAULT_BACKLOG = 16;

namespace tcp {
struct AcceptException : std::exception {
  const char* what() const noexcept { return "Could not accept"; }
};

struct BindException : std::exception {
  const char* what() const noexcept { return "Could not bind"; }
};

struct ListenException : std::exception {
  const char* what() const noexcept { return "Could not listen"; }
};

class Server {
 public:
  Server(int port, int backlog = DEFAULT_BACKLOG);
  ~Server();
  Connection* accept();
  void close();
  int get_port();

 private:
  int socket_fd;
  int port;
  bool open = true;
  std::vector<Connection*> clients;
};
}  // namespace tcp

#endif
