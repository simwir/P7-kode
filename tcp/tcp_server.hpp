#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <string>

constexpr size_t DEFAULT_BACKLOG = 16;

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

class TCPServerSendException : public std::exception {
  const char* what() const noexcept { return "Could not send"; }
};

class TCPServerReceiveException : public std::exception {
  const char* what() const noexcept { return "Could not receive"; }
};

class TCPServer {
 public:
  TCPServer(int port, int backlog = DEFAULT_BACKLOG);
  int accept();
  ssize_t receive(int client_fd, char* message_out, ssize_t size,
                  int flags = 0);
  ssize_t send(int client_fd, std::string mesage);
  void close();
  int get_port();

 private:
  int socket_fd;
  int port;
};

#endif
