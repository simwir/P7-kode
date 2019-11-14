/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <string>

#include <tcp/connection.hpp>
#include <tcp/exception.hpp>

constexpr int BUFFER_SIZE = 256;

ssize_t tcp::Connection::send(const std::string &message, int flags)
{
    if (!ready) {
        throw ConnectionException("Connection not ready");
    }

    if (!open) {
        throw ConnectionException("Connection not open");
    }

    std::string prepped_message = "#|" + message + "|#";

    ssize_t bytes = ::send(fd, prepped_message.c_str(), prepped_message.length(), flags);

    if (bytes == -1) {
        throw tcp::SendException(message);
    }

    return bytes;
}

std::optional<std::string> tcp::Connection::parse_message()
{
    size_t start_pos, end_pos;
    std::string message;
    start_pos = obuffer.find("#|");
    end_pos = obuffer.find("|#");

    if (end_pos != std::string::npos) {
        if (start_pos != 0) {
            throw tcp::MalformedMessageException(obuffer);
        }
        message = obuffer.substr(start_pos + 2, end_pos - 2);
        obuffer.erase(start_pos, end_pos + 2);
        return std::optional{message};
    }
    else {
        return std::nullopt;
    }
}

std::string tcp::Connection::receive_blocking()
{
    auto received = receive(true);
    assert(received.has_value());
    return received.value(); // When called blocking we know a value is present.
}

std::optional<std::string> tcp::Connection::receive_nonblocking()
{
    return receive(false);
}

std::optional<std::string> tcp::Connection::receive(bool blocking)
{
    if (!ready) {
        throw ConnectionException("Connection not ready");
    }

    if (!open) {
        throw ConnectionException("Connection not open");
    }

    auto message = parse_message();
    if (message) {
        return message;
    }

    char buffer[BUFFER_SIZE];

    do {
        std::memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes = ::recv(fd, buffer, BUFFER_SIZE, blocking ? 0 : MSG_DONTWAIT);

        if (bytes == -1) {
            // If the MSG_DONTWAIT flag is set recv might return with -1 and errno to one of
            // the following values indicating that no error occurred, but the recv buffer was empty
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return std::nullopt;
            }
            else {
                throw tcp::ReceiveException(errno);
            }
        }

        obuffer.append(buffer, bytes);
        message = parse_message();
        // repeat if received message was incomplete.
    } while (!message);

    return message;
}

void tcp::Connection::set_fd(int fd)
{
    this->fd = fd;
    ready = true;
}

bool tcp::Connection::closed()
{
    return !ready || !open;
}

void tcp::Connection::close()
{
    if (!open) {
        return;
    }

    if (::close(fd) == -1) {
        throw tcp::CloseException();
    };

    open = false;
}

tcp::Connection::~Connection()
{
    if (open)
        close();
}
