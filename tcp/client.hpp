#ifndef TCP_CLIENT_HPP
#define TCP_CLIENT_HPP

#include <string>

namespace tcp {
class Client {
 public:
  Client(const std::string& host, const std::string& port);
  ssize_t send(const std::string& message);
  std::string receive(int flags = 0);
  void close();

 private:
  int socket_fd;
  std::string host;
  std::string port;
};
}  // namespace tcp

#endif
