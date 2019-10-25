#ifndef RECEIVE_EXCEPTION_HPP
#define RECEIVE_EXCEPTION_HPP

#include <exception>
#include <string>

namespace tcp {
struct CloseException : std::exception {
    const char *what() const noexcept { return "Could not close"; }
};
class UnreadableFunctionException : public std::exception {
    std::string message;

  public:
    UnreadableFunctionException(const std::string &in_message) { message = in_message; }
    const char *what() const noexcept override { return message.c_str(); }
};
class InvalidParametersException : public std::exception {
    std::string message;

  public:
    InvalidParametersException(const std::string &in_message)
    {
        message = "Invalid number of arguments:" + in_message;
    }
    const char *what() const noexcept override { return message.c_str(); }
};
} // namespace tcp

#endif
