#ifndef CONTROLLER_SERVER_HPP
#define CONTROLLER_SERVER_HPP

#include <string>

class controller_server{
  public:
    controller_server(std::string port);
    void send(std::string message);
    void receive(std::string message, int length);
     
  private:
    int server_socket;
};

#endif