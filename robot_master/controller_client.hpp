#ifndef CONTROLLER_CLIENT_HPP
#define CONTROLLER_CLIENT_HPP

#include <string>
#include <utility>
#include <tuple>

class controller_client {
  public:
    controller_client(std::string hostname, std::string port);

    bool set_destination(std::pair<double, double> coordinate);

    std::pair<double, double> get_location();

  private:
    int socketfd;
};

#endif
