#ifndef CONTROLLER_CLIENT_HPP
#define CONTROLLER_CLIENT_HPP

#include <string>
#include <tuple>

class controller_client {
  public:
    controller_client(std::string hostname, std::string port);

    bool set_destination(std::tuple<double, double> coordinate);

    std::tuple<double, double> get_location();

  private:
    int socketfd;
};

#endif
