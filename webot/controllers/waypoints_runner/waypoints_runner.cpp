// File:          waypoints_runner.cpp
// Date:
// Description:
// Author:
// Modifications:

// You may need to add webots include files such as
// <webots/DistanceSensor.hpp>, <webots/Motor.hpp>, etc.
// and/or to add some other includes
#include <webots/Robot.hpp>
#include <webots/Supervisor.hpp>

#include "points.hpp"
#include "robot.hpp"

// This is the main program of your controller.
// It creates an instance of your Robot instance, launches its
// function(s) and destroys it at the end of the execution.
// Note that only one instance of Robot should be created in
// a controller program.
// The arguments of the main function can be specified by the
// "controllerArgs" field of the Robot node
int main(int argc, char **argv)
{
    // create the Robot instance.
    robot_controller controller{new webots::Supervisor()};
    controller.run_simulation();

    // Main loop:
    // - perform simulation steps until Webots is stopping the controller
    // Enter here exit cleanup code.

    return 0;
}
