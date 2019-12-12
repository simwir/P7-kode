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
#ifndef ORCHESTRATOR_CLOCK_HPP
#define ORCHESTRATOR_CLOCK_HPP

#include "tcp/client.hpp"
#include <string>
#include <iostream>

#define TRACEME
#include "trace.def"

namespace robot {
class Clock {
  public:
    virtual int get_current_time() = 0;

    virtual ~Clock() = default;
};

class WebotsClock : public Clock {
  public:
    WebotsClock(const std::string &host, const std::string &port);

    int get_current_time() override;

    void stop_clock() {
        TRACE(std::cout << "========stopping time=========" << std::endl);
        webots_client.send("stop_time");
    };
    void start_clock() {
        TRACE(std::cout << "========starting time=========" << std::endl);
        webots_client.send("start_time"); };

  private:
    tcp::Client webots_client;
};

} // namespace robot
#endif
