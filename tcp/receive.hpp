#ifndef RECEIVE_HPP
#define RECEIVE_HPP

#include <exception>
#include <string>
#include <vector>

namespace tcp {
class MalformedMessageException : public std::exception {
  std::string message;

 public:
  MalformedMessageException(const std::string& in_message) {
    message = in_message;
  }

  const char* what() const noexcept {
    return ("Message malformed: " + message).c_str();
  }
};

struct ReceiveException : std::exception {
  const char* what() const noexcept {
    return "Could not receive from file descriptor";
  }
};

std::vector<std::string> receive(int socket_fd, int flags = 0);
}  // namespace tcp

#endif
