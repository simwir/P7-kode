#include "tcp_server.hpp"

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

#define DEFAULT_BACKLOG 16

TCPServer::TCPServer(int port, int backlog = DEFAULT_BACKLOG) {
  sockaddr_in server_address;

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_fd == -1) {
    throw TCPServerSocketException();
  }

  memset(&server_address, 0, sizeof server_address);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(port);

  if (bind(socket_fd, (sockaddr *)&server_address, sizeof server_address) == -1) {
    throw TCPServerBindException();
  }

  if (listen(socket_fd, backlog) == -1) {
    throw TCPServerListenException();
  }
}

int TCPServer::accept() {
  sockaddr_in client_address;
  int fd = ::accept(socket_fd, (sockaddr *)&client_address,
                    (socklen_t *)sizeof client_address);

  if (fd == -1) {
    throw TCPServerAcceptException();
  }

  return fd;
}

void TCPServer::receive(int client_fd, char *message_out) {
  ::read(client_fd, message_out, sizeof message_out);
}

void TCPServer::send(int client_fd, std::string message) {
  if (::send(client_fd, message.c_str(), message.length(), 0) == -1) {
  }
}

void TCPServer::close() {
  if (::close(socket_fd) == -1) {
    throw TCPServerCloseException();
  }
}
