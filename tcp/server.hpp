#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <list>
#include <string>

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
  int accept();
  std::string receive(int client_fd, int flags = 0);
  ssize_t send(int client_fd, std::string mesage);
  void close();
  void close_client(int client_fd);
  int get_port();

 private:
  int socket_fd;
  int port;
  std::list<int> clients;
};
}  // namespace tcp

#endif
