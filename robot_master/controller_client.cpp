#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <utility>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "controller_client.hpp"

controller_client::controller_client(std::string host, std::string port)
{
  struct addrinfo hints, *res;

  // first, load up address structs with getaddrinfo():
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  getaddrinfo(host, port, &hints, &res);

  // make a socket:
  socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  // connect!
  connect(socketfd, res->ai_addr, res->ai_addrlen);
}


bool controller_client::set_destination(std::pair<double, double> coordinate)
{
  std::string message;

  message << "set_destination "
          << std::get<0>(coordinate)
          << " "
          << std::get<1>(coordinate)
          << "\n";

  return send(socketfd, message.c_str(), message.length(), 0) != -1;
}

std::pair<double, double> controller_client::get_location()
{
  std::string message;

  message << "get_destination\n";

  return send(socketfd, message.c_str(), message.length(), 0);
}
