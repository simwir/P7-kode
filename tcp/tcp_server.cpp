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

TCPServer::TCPServer(int socket_fd) {

}

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

  if (bind(socket_fd,
          (sockaddr *)&server_address,
          sizeof server_address) == -1) {
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

  socklen_t client_length = sizeof client_address;
  int fd = ::accept(socket_fd, (sockaddr *)&client_address, &client_length);

  if (fd == -1) {
    throw TCPServerAcceptException();
  }

  clients.push_back(fd);

  return fd;
}

std::string TCPServer::receive(int c/ient_fd, int flags) {
  std::string output;
  char buffer[256];

  while (true) {
    memset(buffer, 0, sizeof buffer);
    ssize_t bytes = recv(client_fd, buffer, sizeof buffer, flags);

    if (bytes == -1) {
      throw TCPServerReceiveException();
    } else if (bytes == 0) {
      break;
    } else {
      output.append(buffer, bytes);
    }
  }

  int start_pos = output.find('#');
  int end_pos = output.find_last_of('#');

  if (start_pos == std::string::npos || end_pos == std::string::npos) {
    throw TCPServerMalformedMessageException(output);
  }

  return output.substr(start_pos + 1, end_pos - start_pos - 1);
}

ssize_t TCPServer::send(int client_fd, std::string message) {
  ssize_t bytes = ::send(client_fd, message.c_str(), message.length(), 0);

  if (bytes == -1) {
    throw TCPServerSendException();
  }

  return bytes;
}

void TCPServer::close_client(int client_fd) {
  if (std::find(clients.begin(), clients.end(), client_fd) != clients.end()) {
    ::close(client_fd);
    clients.remove(client_fd);
  }
}

void TCPServer::close() {
  if (::close(socket_fd) == -1) {
    throw TCPServerCloseException();
  }
}

int TCPServer::get_port() { return port; }
