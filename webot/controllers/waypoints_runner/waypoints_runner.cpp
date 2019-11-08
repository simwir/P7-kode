#include <webots/Robot.hpp>
#include <webots/Supervisor.hpp>

#include "geo/geo.hpp"
#include "robot.hpp"

int main(int argc, char **argv)
{
    RobotController controller{new webots::Supervisor()};
    controller.run_simulation();

    return 0;
}
