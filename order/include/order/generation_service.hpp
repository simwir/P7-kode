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

#ifndef ORDER_GENERATION_SERVICE_HPP
#define ORDER_GENERATION_SERVICE_HPP

#include "order/generator.hpp"
#include "tcp/connection.hpp"
#include "tcp/server.hpp"
#include <memory>
#include <mutex>

namespace order {
class GenerationService {
    tcp::Server server;
    std::shared_ptr<Generator> generator;
    void parse_message(std::shared_ptr<tcp::Connection> connection);
    int max_order_count;
    int order_count = 0;
    std::mutex order_count_mutex;

  public:
    GenerationService(int port, std::shared_ptr<Generator> generator, int max_order_count = -1);
    void start();
    int get_port();
};
} // namespace order

#endif
