#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <string>

#include <tcp_exception.hpp>
#include <server.hpp>

tcp::Server::Server(int in_port, int backlog) {
  sockaddr_in server_address;
  socklen_t length = sizeof(sockaddr);

  // Should getaddrinfo() be called here?
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  std::memset(&server_address, 0, sizeof server_address);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(in_port);

  if (bind(socket_fd, (sockaddr*) &server_address, length) ==
      -1) {
    throw tcp::BindException();
  }

  getsockname(socket_fd, (sockaddr*) &server_address, &length);

  port = ntohs(server_address.sin_port);

  if (listen(socket_fd, backlog) == -1) {
    throw tcp::ListenException();
  }
}

std::shared_ptr<tcp::Connection> tcp::Server::accept() {

  if (!open) {
    throw ConnectionException("Connection not open");
  }

  sockaddr_in client_address;
  socklen_t length = sizeof(sockaddr);

  int fd = ::accept(socket_fd, (sockaddr*) &client_address, &length);

  if (fd == -1) {
    throw tcp::AcceptException();
  }
  auto con = std::make_shared<tcp::Connection>(fd);

  clients.push_back(con->weak_from_this());

  return con;
}

void tcp::Server::close() {
  if (!open || ::close(socket_fd) == -1) {
    throw tcp::CloseException();
  }

  for (auto connection : clients) {
    if (auto spt = connection.lock()) {
      spt->open = false;
    }
  }
}

int tcp::Server::get_port() {
    return port;
}

tcp::Server::~Server() {
    close();
};
