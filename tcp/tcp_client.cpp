#include "tcp_client.hpp"

#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>
#include <string>

/**
 * Creates a TCP connection to a host on a given port and connects.
 */
TCPClient::TCPClient(std::string host, std::string port) {
  struct addrinfo hints, *res;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  getaddrinfo(host.c_str(), port.c_str(), &hints, &res);

  socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  connect(socket_fd, res->ai_addr, res->ai_addrlen);
}

bool TCPClient::send(std::string message) {
  if (::send(socket_fd, (char *)message.c_str(), message.length(), 0) == -1) {
    throw TCPSendException(message);
  }

  return true;
}

void TCPClient::receive(std::string &out_message) {
  if (recv(socket_fd, (char *)out_message.c_str(), out_message.length(), 0) ==
      -1) {
    throw TCPReceiveException();
  }
}
