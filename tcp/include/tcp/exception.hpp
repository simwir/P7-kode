/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler
 *Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 *associated documentation files (the "Software"), to deal in the Software without restriction,
 *including without limitation the rights to use, copy, modify, merge, publish, distribute,
 *sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or
 *substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 *NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
 *OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef RECEIVE_EXCEPTION_HPP
#define RECEIVE_EXCEPTION_HPP

#include <string>

namespace tcp {
struct CloseException : std::exception {
    const char *what() const noexcept { return message.c_str(); }
    std::string message;
    CloseException(int _errno) : message("Could not close. Errno: " + std::to_string(_errno)) {}
};

class MessageException : public std::exception {
    std::string message;

  public:
    MessageException(const std::string &in_message) : message(in_message) {}

    const char *what() const noexcept { return message.c_str(); }
};

class ConnectionException : public std::exception {
    std::string message;

  public:
    ConnectionException(const std::string &in_message) : message(in_message) {}

    const char *what() const noexcept { return message.c_str(); }
};
} // namespace tcp

#endif
