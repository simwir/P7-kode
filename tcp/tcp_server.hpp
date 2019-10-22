#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <string>
#include <list>
#include <algorithm>

constexpr size_t DEFAULT_BACKLOG = 16;

struct TCPServerSocketException : public std::exception {
  const char* what() const noexcept { return "Could not create socket"; }
};

struct TCPServerBindException : public std::exception {
  const char* what() const noexcept { return "Could not bind socket"; }
};

struct TCPServerListenException : public std::exception {
  const char* what() const noexcept { return "Could not listen"; }
};

struct TCPServerCloseException : public std::exception {
  const char* what() const noexcept { return "Could not close"; }
};

struct TCPServerAcceptException : public std::exception {
  const char* what() const noexcept { return "Could not accept"; }
};

struct TCPServerSendException : public std::exception {
  const char* what() const noexcept { return "Could not send"; }
};

struct TCPServerReceiveException : public std::exception {
  const char* what() const noexcept { return "Could not receive"; }
};

struct TCPServerMalformedMessageException : public std::exception {
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
  TCPServer(int socket_fd);
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
