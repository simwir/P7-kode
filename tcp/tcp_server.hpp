#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <exception>
#include <string>

class TCPServerSocketException : public std::exception {
  const char* what() const noexcept { return "Could not create socket"; }
};

class TCPServerBindException : public std::exception {
  const char* what() const noexcept { return "Could not bind socket"; }
};

class TCPServerListenException : public std::exception {
  const char* what() const noexcept { return "Could not listen"; }
};

class TCPServerCloseException : public std::exception {
  const char* what() const noexcept { return "Could not close"; }
};

class TCPServer {
 public:
  TCPServer(int port);
  void close();

 private:
  int socket_fd;
};

#endif
