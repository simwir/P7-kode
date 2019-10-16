#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "controller_client.hpp"

std::vector<std::string> split(const std::string &input, char delimiter) {
  std::vector<std::string> result;
  size_t current, previous = 0;
  current = input.find(delimiter);

  while (current != std::string::npos) {
    result.push_back(input.substr(previous, current - previous));
    previous = current + 1;
    current = input.find(delimiter, previous);
  }
  result.push_back(input.substr(previous, current - previous));

  return result;
}

controller_client::controller_client(std::string host, std::string port) {
  struct addrinfo hints, *res;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  getaddrinfo(host.c_str(), port.c_str(), &hints, &res);

  socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  connect(socketfd, res->ai_addr, res->ai_addrlen);
}

bool controller_client::set_destination(std::pair<double, double> coordinate) {
  std::stringstream message;

  message << "set_destination " << std::get<0>(coordinate) << " "
          << std::get<1>(coordinate) << std::endl;

  return send(socketfd, message.str().c_str(), message.str().length(), 0) != -1;
}

std::pair<double, double> controller_client::get_location() {
  int bytes_sent;
  std::vector<std::string> result;
  std::string received(256, ' ');
  std::string message("get_location");

  bytes_sent = send(socketfd, message.c_str(), message.length(), 0);

  recv(socketfd, (char *)received.c_str(), received.length(), 0);

  result = split(received, ' ');

  if (result.size() != 3) {
    // Throw exception
  }

  if (result.at(0) == "location") {
    double x = std::stod(result.at(1));
    double y = std::stod(result.at(2));

    return std::make_pair(x, y);
  } else {
    return std::make_pair(0.0, 0.0);
  }
}

int main(void) {
  std::string input = "1 2 3";
  std::vector<std::string> test = split(input, ' ');
  std::copy(test.begin(), test.end(),
            std::ostream_iterator<std::string>(std::cout, "\n"));
  return 0;
}
