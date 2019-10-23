#include "send.hpp"

#include <sys/socket.h>
#include <sys/types.h>

ssize_t tcp::send(int fd, const std::string& message) {
  std::string prepped_message = "#|" + message + "|#";

  ssize_t bytes =
      ::send(fd, prepped_message.c_str(), prepped_message.length(), 0);

  if (bytes == -1) {
    throw tcp::SendException(message);
  }

  return bytes;
}
