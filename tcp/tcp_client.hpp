#ifndef TCP_CLIENT_HPP
#define TCP_CLIENT_HPP

#include <exception>
#include <iostream>
#include <string>
#include <vector>

class TCPCloseException : public std::exception {
  const char* what() const noexcept { return "Could not close"; }
};

class TCPClient {
 public:
  TCPClient(const std::string& host, const std::string& port);
  ssize_t send(const std::string& message);
  ssize_t receive(char* message_out, ssize_t size, int flags = 0);
  void close();

 private:
  int socket_fd;
  std::string host;
  std::string port;
};

#endif
