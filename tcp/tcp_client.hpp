#ifndef TCP_CLIENT_HPP
#define TCP_CLIENT_HPP

#include <exception>
#include <iostream>
#include <string>
#include <vector>

class TCPSendException : public std::exception {
  std::string message;
  const char* what() const noexcept { return message.c_str(); }

 public:
  TCPSendException(const std::string& in_message) { message = in_message; }
};

class TCPReceiveException : public std::exception {
  const char* what() const noexcept { return "Could not receive message"; }
};

class TCPCloseException : public std::exception {
  const char* what() const noexcept { return "Could not close"; }
};

class TCPClient {
 public:
  TCPClient(std::string host, std::string port);
  ssize_t send(std::string message);
  ssize_t receive(char* message_out, ssize_t size, int flags = 0);
  void close();

 private:
  int socket_fd;
};

#endif
