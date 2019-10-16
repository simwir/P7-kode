#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "controller_server.hpp"

#define BACKLOG 1     // how many pending connections queue will hold

controller_server::controller_server(std::string port)
{
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int sockfd, new_fd;

    // first, load up address structs with getaddrinfo():

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    getaddrinfo(NULL, port.c_str(), &hints, &res);

    // make a socket, bind it, and listen on it:

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    bind(sockfd, res->ai_addr, res->ai_addrlen);
    listen(sockfd, BACKLOG);

    // now accept an incoming connection:

    addr_size = sizeof their_addr;
    server_socket = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
    fcntl(server_socket, F_SETFL, O_NONBLOCK);
}

void controller_server::send(std::string message){
  send(server_socket, message.c_str(), message.length(), 0);
}

void controller_server::receive(std::string message, int length){
  recv(server_socket, (char *)message.c_str(), length, 0);
}