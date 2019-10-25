#ifndef TCP_CLIENT_HPP
#define TCP_CLIENT_HPP

#include <string>

#include "connection.hpp"

namespace tcp {
class Client : public Connection {
 public:
  Client(const std::string& host, const std::string& port);

 private:
  std::string host;
  std::string port;
};
}  // namespace tcp

#endif
