#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstring>
#include <iostream>
#include <string>

#include <tcp/client.hpp>

/**
 * Creates a TCP connection to a host on a given port and connects.
 */
tcp::Client::Client(const std::string &host, const std::string &port) : host(host), port(port)
{
    addrinfo hints, *res;

    std::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int err = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
    if (err != 0) {
        throw tcp::ConnectException{gai_strerror(err)};
    }

    int socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    connect(socket_fd, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);

    set_fd(socket_fd);
}
