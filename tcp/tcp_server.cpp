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

  if (bind(socket_fd, (sockaddr *)&server_address, sizeof server_address) ==
      -1) {
    throw TCPServerBindException();
  }

  if (listen(socket_fd, backlog) == -1) {
    throw TCPServerListenException();
  }
}

void TCPServer::close() {
  if (::close(socket_fd) == -1) {
    throw TCPServerCloseException();
  }
}
