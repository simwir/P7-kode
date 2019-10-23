#ifndef SEND_EXCEPTION_HPP
#define SEND_EXCEPTION_HPP

#include <exception>
#include <string>

namespace tcp {
class SendException : public std::exception {
  std::string message;

 public:
  SendException(const std::string& in_message) : message(in_message){};

  const char* what() const noexcept {
    return std::string("Could not send message: " + message).c_str();
  }
};

ssize_t send(int fd, const std::string& message);
}  // namespace tcp

#endif
