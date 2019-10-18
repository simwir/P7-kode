#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <list>
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

class TCPServerMalformedMessageException : public std::exception {
  std::string message;
  const char* what() const noexcept {
    return ("Message malformed: " + message).c_str();
  }

 public:
  TCPServerMalformedMessageException(const std::string& in_message) {
    message = in_message;
  }
};

class TCPServer {
 public:
  TCPServer(int port, int backlog = DEFAULT_BACKLOG);
  int accept();
  std::string receive(int client_fd, int flags = 0);
  ssize_t send(int client_fd, std::string mesage);
  void close();
  void close_client(int client_fd);
  int get_port();

 private:
  int socket_fd;
  int port;
  std::list<int> clients;
};

#endif
