#ifndef TCP_CONNECTION_HPP
#define TCP_CONNECTION_HPP

#include <string>
#include <vector>
#include <sstream>
#include <memory>

namespace tcp {

class SendException : public std::exception {
  std::string message;

public:
  SendException(const std::string& in_message) : message(in_message) { }

  const char* what() const noexcept {
    return std::string("Could not send message: " + message).c_str();
  }
};

class ReceiveException : public std::exception {
  int err;

public:
  ReceiveException(int err) : err(err) { }

  const char* what() const noexcept {
    std::stringstream ss;
    ss << "Could not read receive buffer: " << err;
    return ss.str().c_str();
  }
};

struct MalformedMessageException : public std::exception {
  std::string message;

public:
  MalformedMessageException(const std::string& in_message) : message(in_message) { }
  
  const char* what() const noexcept {
    return std::string("Missing start sequence: " + message).c_str();
  }
};

class Connection : public std::enable_shared_from_this<Connection> {
public:
    Connection(int fd) : fd(fd) { ready = true; }
    Connection() { }
    ~Connection();
    std::vector<std::string> receive(int flags = 0);
    ssize_t send(const std::string& message);
    void close();
    
    bool closed();
protected:
    void setFD(int fd);
private:
    int fd;
    std::string obuffer;
    bool open = true;
    bool ready;
    
    friend class Server; 
};
    
}

#endif // TCP_CONNECTION_HPP