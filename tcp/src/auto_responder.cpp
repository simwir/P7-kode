#include "tcp/server.hpp"

#include <iostream>
#include <sstream>
#include <thread>

using namespace std;
using tcp::Connection;
using tcp::Server;

static string prog_name;

void usage(int code = 0)
{
    std::cerr << "Usage: " << prog_name << " <port> [<response>...]" << std::endl;
    exit(code);
}


int main(int argc, char *argv[])
{
    prog_name = argv[0];
    if (argc <= 2) {
        usage(-1);
    }
    int port;
    stringstream ss{argv[1]};
    ss >> port;
    stringstream _response;
    for (int i = 2; i < argc; ++i) {
        _response << argv[i];
        if (i < argc - 1) _response << ' ';
    }
    string response = _response.str();
    if (!ss) {
        std::cerr << "Couldn't parse valid port from " << argv[1];
        usage(1);
    }

    Server server{port};
    while (true) {
        auto conn = server.accept();
        thread t{[response](shared_ptr<Connection> con){
                     while (true) {
                         con->receive_blocking();
                         con->send(response);
                     }
                 }, conn};
        t.detach();
    }
}
