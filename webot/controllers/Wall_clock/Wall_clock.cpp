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
#include "tcp/exception.hpp"
#include <tcp/server.hpp>
#include <thread>
#include <webots/Robot.hpp>
#include <webots/Supervisor.hpp>

#include <cmath>
#include <iostream>
#include <mutex>

using namespace webots;

void connection(std::shared_ptr<tcp::Connection> connection, Supervisor *robot);
void accepter(Supervisor *robot);
int stop_semaphore = 0;
std::mutex stop_mutex;

int main(int argc, char **argv)
{
    Supervisor *robot = new Supervisor();
    int time_step = (int)robot->getBasicTimeStep();
    std::thread accept_thread{accepter, robot};
    while (robot->step(time_step) != -1) {
    };

    delete robot;
    return 0;
}

void accepter(Supervisor *robot)
{
    tcp::Server server{5555};
    while (true) {
        auto conn = server.accept();
        std::thread t{connection, conn, robot};
        t.detach();
    }
}

void connection(std::shared_ptr<tcp::Connection> connection, Supervisor *robot)
{
    try {
        while (true) {
            auto message = connection->receive_blocking();
            // Convert double seconds to long millis.
            if (message == "get_time") {
                connection->send(std::to_string(std::lround(robot->getTime() * 1000)));
            }
            else if (message == "stop_time") {
                std::scoped_lock _{stop_mutex};
                stop_semaphore++;
                std::cout << "Pausing" << std::endl;
                robot->simulationSetMode(Supervisor::SimulationMode::SIMULATION_MODE_PAUSE);
            }
            else if (message == "start_time") {
                std::scoped_lock _{stop_mutex};
                stop_semaphore--;
                std::cout << "Start request" << std::endl;
                if (stop_semaphore == 0) {
                    std::cout << "Unpausing" << std::endl;
                    robot->simulationSetMode(Supervisor::SimulationMode::SIMULATION_MODE_REAL_TIME);
                    robot->step(time_step);
                }
            }
        }
    }
    catch (tcp::ConnectionClosedException &e) {
        std::cerr << "Connection closed." << std::endl;
    }
    catch (tcp::CloseException &e) {
        std::cerr << "Connection closed." << std::endl;
    }
}
