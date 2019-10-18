#ifndef SEND_EXCEPTION_HPP
#define SEND_EXCEPTION_HPP

#include <exception>
#include <string>

namespace tcp {
class SendException : public std::exception {
  const char* what() const noexcept {
    return std::string("Could not send message: " + message).c_str();
  }

 public:
  SendException(const std::string& in_message) : message(in_message){};

 private:
  std::string message;
};
}  // namespace tcp

#endif
