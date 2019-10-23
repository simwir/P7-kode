#include "receive.hpp"

#include <sys/socket.h>
#include <sys/types.h>

#include <iostream>
#include <string>
#include <cstring>

/**
 * Reads the message in a socket buffer until empty.
 */
std::vector<std::string> tcp::receive(int socket_fd, int flags) {
  std::string output;
  char buffer[256];

  while (true) {
    std::memset(buffer, 0, sizeof buffer);
    ssize_t bytes = ::recv(socket_fd, buffer, sizeof buffer, flags);

    if (bytes == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK){
            return std::vector<std::string>{};
        } else {
            throw tcp::ReceiveException();
        }
    } else if (bytes == 0) {
      break;
    } else {
      output.append(buffer, bytes);
    }
  }

  size_t start_pos, end_pos;
  std::vector<std::string> messages;

  do {
      start_pos = output.find("#|");
      end_pos = output.find("|#");

      if (start_pos != std::string::npos && end_pos == std::string::npos)
          throw tcp::MalformedMessageException(output);
      messages.push_back(output.substr(start_pos + 2, end_pos - 1));
      output.erase(start_pos, end_pos + 1);
  } while (start_pos != std::string::npos);

  return messages;
}
