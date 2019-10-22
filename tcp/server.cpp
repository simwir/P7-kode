#include "server.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <iostream>

#include "close_exception.hpp"
#include "receive.hpp"
#include "send.hpp"

tcp::Server::Server(int in_port, int backlog) {
  sockaddr_in server_address;

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  memset(&server_address, 0, sizeof server_address);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(in_port);

  if (bind(socket_fd, (sockaddr *)&server_address, sizeof server_address) ==
      -1) {
    throw tcp::BindException();
  }

  socklen_t length = sizeof server_address;

  getsockname(socket_fd, (sockaddr *)&server_address, &length);

  port = ntohs(server_address.sin_port);

  if (listen(socket_fd, backlog) == -1) {
    throw tcp::ListenException();
  }
}

int tcp::Server::accept() {
  sockaddr_in client_address;

  socklen_t client_length = sizeof client_address;
  int fd = ::accept(socket_fd, (sockaddr *)&client_address, &client_length);

  if (fd == -1) {
    throw tcp::AccpetException();
  }

  clients.push_back(fd);

  return fd;
}

std::string tcp::Server::receive(int client_fd, int flags) {
  return tcp::receive(client_fd, flags);
}

ssize_t tcp::Server::send(int client_fd, const std::string message) {
  return tcp::send(client_fd, message);
}

void tcp::Server::close_client(int client_fd) {
  if (std::find(clients.begin(), clients.end(), client_fd) != clients.end()) {
    ::close(client_fd);
    clients.remove(client_fd);
  }
}

void tcp::Server::close() {
  if (::close(socket_fd) == -1) {
    throw tcp::CloseException();
  }
}

int tcp::Server::get_port() { return port; }
