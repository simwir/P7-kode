#include "receive.hpp"

#include <sys/socket.h>
#include <sys/types.h>

#include <iostream>
#include <string>
#include <cstring>

/**
 * Reads the message in a socket buffer until empty.
 */
std::string tcp::receive(int socket_fd, int flags) {
  std::string output;
  char buffer[256];

  while (true) {
    memset(buffer, 0, sizeof buffer);
    ssize_t bytes = ::recv(socket_fd, buffer, sizeof buffer, flags);

    if (bytes == -1) {
      throw tcp::ReceiveException();
    } else if (bytes == 0) {
      break;
    } else {
      output.append(buffer, bytes);
    }
  }


  int start_pos = output.find('#');
  int end_pos = output.find('#', start_pos + 1);

  if (start_pos == std::string::npos || end_pos == std::string::npos) {
    throw tcp::MalformedMessageException(output);
  }

  return output.substr(start_pos + 1, end_pos - start_pos - 1);
}
