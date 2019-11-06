#ifndef TCP_CONNECTION_HPP
#define TCP_CONNECTION_HPP

#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <optional>

namespace tcp {

class SendException : public std::exception {
    std::string message;

  public:
    SendException(const std::string &in_message) : message("Could not send message: " + in_message)
    {
    }

    const char *what() const noexcept { return message.c_str(); }
};

class ReceiveException : public std::exception {
    int err = 0;
    std::string msg;

  public:
    ReceiveException(int err)
        : err(err), msg("Could not read receive buffer: " + std::to_string(err))
    {
    }

    ReceiveException() : msg("Got 0 bytes") {}

    const char *what() const noexcept { return msg.c_str(); }
};

struct MalformedMessageException : public std::exception {
    std::string message;

  public:
    MalformedMessageException(const std::string &in_message)
        : message(std::string("Missing start sequence: " + in_message))
    {
    }

    const char *what() const noexcept { return message.c_str(); }
};

class Connection : public std::enable_shared_from_this<Connection> {
  public:
    Connection(int fd) : fd(fd) { ready = true; }
    Connection() {}
    virtual ~Connection();
    std::optional<std::string> receive_nonblocking();
    std::string receive_blocking();
    ssize_t send(const std::string &message, int flags = 0);
    void close();

    bool closed();

  protected:
    void set_fd(int fd);
    std::optional<std::string> parse_message();

  private:
    std::optional<std::string> receive(bool blocking);
    int fd;
    std::string obuffer;
    bool open = true;
    bool ready;

    friend class Server;
};

} // namespace tcp

#endif // TCP_CONNECTION_HPP
