#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

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

void tcp::Connection::read_buffer(int flags)
{
    char buffer[BUFFER_SIZE];

    while (true) {
        std::memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes = ::recv(fd, buffer, BUFFER_SIZE, flags | MSG_DONTWAIT);

        if (bytes == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            else {
                throw tcp::ReceiveException(errno);
            }
        }
        else if (bytes == 0) {
            throw tcp::ReceiveException(); // If this happens, wud
        }
        else {
            obuffer.append(buffer, bytes);
        }
    }
}

std::vector<std::string> tcp::Connection::parse_messages()
{
    size_t start_pos, end_pos;
    std::vector<std::string> messages;
    start_pos = obuffer.find("#|");
    end_pos = obuffer.find("|#");

    while (end_pos != std::string::npos) {
        if (start_pos != 0) {
            throw tcp::MalformedMessageException(obuffer);
        }

        messages.push_back(obuffer.substr(start_pos + 2, end_pos - 2));
        obuffer.erase(start_pos, end_pos + 2);

        start_pos = obuffer.find("#|");
        end_pos = obuffer.find("|#");
    }

    return messages;
}

std::vector<std::string> tcp::Connection::receive(int flags)
{
    if (!ready) {
        throw ConnectionException("Connection not ready");
    }

    if (!open) {
        throw ConnectionException("Connection not open");
    }

    read_buffer(flags);
    return parse_messages();
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
