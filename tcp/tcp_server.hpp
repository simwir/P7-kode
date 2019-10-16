#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

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

class TCPServerAcceptException : public std::exception {
  const char* what() const noexcept { return "Could not accept"; }
};

class TCPServer {
 public:
  TCPServer(int port);
  int accept();
  void receive(int client_fd, char* message_out);
  void send(int client_fd, std::string mesage);
  void close();

 private:
  int socket_fd;
};

#endif
