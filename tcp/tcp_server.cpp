#include "tcp_server.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

TCPServer::TCPServer(int in_port, int backlog) {
  sockaddr_in server_address;

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_fd == -1) {
    throw TCPServerSocketException();
  }

  memset(&server_address, 0, sizeof server_address);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(in_port);

  if (bind(socket_fd, (sockaddr *)&server_address, sizeof server_address) ==
      -1) {
    throw TCPServerBindException();
  }

  socklen_t length = sizeof server_address;

  getsockname(socket_fd, (sockaddr *)&server_address, &length);

  port = ntohs(server_address.sin_port);

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

ssize_t TCPServer::receive(int client_fd, char *message_out, ssize_t size,
                           int flags) {
  ssize_t bytes = recv(client_fd, message_out, size, flags);

  if (bytes == -1) {
    throw TCPServerReceiveException();
  }

  return bytes;
}

ssize_t TCPServer::send(int client_fd, std::string message) {
  ssize_t bytes = ::send(client_fd, message.c_str(), message.length(), 0);

  if (bytes == -1) {
    throw TCPServerSendException();
  }

  return bytes;
}

void TCPServer::close() {
  if (::close(socket_fd) == -1) {
    throw TCPServerCloseException();
  }
}

int TCPServer::get_port() { return port; }
