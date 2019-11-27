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

#include "order/generation_service.hpp"
#include <iostream>
#include <memory>
#include <thread>

namespace order {
GenerationService::GenerationService(int port, Generator &generator)
    : server(port), generator(generator){};

void GenerationService::parse_message(std::shared_ptr<tcp::Connection> connection)
{
    std::string message = connection->receive_blocking();
    Order order = generator.generate_order(std::stoi(message));
    connection->send(order.to_json().toStyledString());
}

void GenerationService::start()
{
    while (true) {
        try {
            std::shared_ptr<tcp::Connection> connection = server.accept();
            std::thread thread{&GenerationService::parse_message, this, connection};
            thread.detach();
        }
        catch (tcp::AcceptException &error) {
            std::cerr << error.what() << std::endl;
        }
        catch (tcp::ConnectionClosedException &_) {
            std::cout << "Connection closed." << std::endl;
            break;
        }
    }
}
} // namespace order
