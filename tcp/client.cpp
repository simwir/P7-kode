#include "client.hpp"

#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>

#include "close_exception.hpp"
#include "receive.hpp"
#include "send.hpp"

/**
 * Creates a TCP connection to a host on a given port and connects.
 */
tcp::Client::Client(const std::string& host, const std::string& port)
    : host(host), port(port) {
  addrinfo hints, *res;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  getaddrinfo(host.c_str(), port.c_str(), &hints, &res);

  socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  connect(socket_fd, res->ai_addr, res->ai_addrlen);
  freeaddrinfo(res);
  freeaddrinfo(&hints);
}

ssize_t tcp::Client::send(const std::string& message) {
  return tcp::send(socket_fd, message);
}

std::string tcp::Client::receive(int flags) {
  return tcp::receive(socket_fd, flags);
}

void tcp::Client::close() {
  if (::close(socket_fd) == -1) {
    throw tcp::CloseException();
  };
}
