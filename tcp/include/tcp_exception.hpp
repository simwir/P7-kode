#ifndef RECEIVE_EXCEPTION_HPP
#define RECEIVE_EXCEPTION_HPP

namespace tcp {
struct CloseException : std::exception {
  const char* what() const noexcept { return "Could not close"; }
};

class ConnectionException : public std::exception {    
  std::string message;

public:
  ConnectionException(const std::string& in_message) : message(in_message) { }
    
  const char* what() const noexcept {
    return message.c_str();
  }
};
}  // namespace tcp

#endif
