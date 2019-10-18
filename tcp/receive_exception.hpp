#ifndef RECEIVE_EXCEPTION_HPP
#define RECEIVE_EXCEPTION_HPP

#include <exception>
#include <string>

namespace tcp {
class ReceiveException : public std::exception {
  const char* what() const noexcept {
    return "Could not receive from file descriptor";
  }
};
}  // namespace tcp

#endif
