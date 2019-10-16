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

class TCPClient {
 public:
  TCPClient(std::string host, std::string port);
  bool send(std::string message);
  void receive(std::string& out_message);

 private:
  int socket_fd;
};

#endif
