#ifndef RECEIVE_EXCEPTION_HPP
#define RECEIVE_EXCEPTION_HPP

#include <exception>

namespace tcp {
struct CloseException : std::exception {
  const char* what() const noexcept { return "Could not close"; }
};
}  // namespace tcp

#endif
