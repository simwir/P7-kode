#include <tcp/server.hpp>
#include <thread>
#include <webots/Robot.hpp>

#include <iostream>

using namespace webots;

void connection(std::shared_ptr<tcp::Connection> connection, Robot *robot);
void accepter(Robot *robot);

int main(int argc, char **argv)
{
    Robot *robot = new Robot();
    int time_step = (int)robot->getBasicTimeStep();
    std::thread accept_thread{accepter, robot};
    while (robot->step(time_step) != -1) {
    };

    delete robot;
    return 0;
}

void accepter(Robot *robot)
{
    tcp::Server server{5555};
    while (true) {
        auto conn = server.accept();
        std::thread t{connection, conn, robot};
        t.detach();
    }
}

void connection(std::shared_ptr<tcp::Connection> connection, Robot *robot)
{
    while (true) {
        connection->receive_blocking();
        connection->send(std::to_string(robot->getTime()));
    }
}
