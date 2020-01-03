/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler
 *Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 *associated documentation files (the "Software"), to deal in the Software without restriction,
 *including without limitation the rights to use, copy, modify, merge, publish, distribute,
 *sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or
 *substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 *NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
 *OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

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
        if (i < argc - 1)
            _response << ' ';
    }
    string response = _response.str();
    if (!ss) {
        std::cerr << "Couldn't parse valid port from " << argv[1];
        usage(1);
    }

    Server server{port};
    while (true) {
        auto conn = server.accept();
        thread t{[response](shared_ptr<Connection> con) {
                     try {
                         while (true) {
                             std::cout << con->receive_blocking() << std::endl;
                             con->send(response);
                         }
                     }
                     catch (tcp::ReceiveException &e) {
                         std::cout << "Connection lost." << std::endl;
                     }
                     catch (tcp::ConnectionClosedException &e) {
                         std::cout << "Connection closed." << std::endl;
                     }
                 },
                 conn};
        t.detach();
    }
}
