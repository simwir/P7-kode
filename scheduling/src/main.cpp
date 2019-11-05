// POSIX includes
#include <unistd.h>

// Other includes
#include <iostream>

#include <waypoint_scheduler.hpp>

class LogWaypointScheduleSubscriber : public scheduling::WaypointScheduleSubscriber {
    void newSchedule(const std::vector<scheduling::Action> &schedule) override
    {
        std::time_t result = std::time(nullptr);
        std::cout << "Got new waypoint schedule at " << std::asctime(std::localtime(&result));

        for (auto action : schedule) {
            std::cout << "Action: ";
            if (action.type == scheduling::ActionType::Hold) {
                std::cout << "Hold";
            }
            else if (action.type == scheduling::ActionType::Waypoint) {
                std::cout << "Waypoint";
            }
            else {
                std::cout << "Unknown";
            }
            std::cout << ", Value: " << action.value << std::endl;
        }
    }
};

int main()
{
    std::cout << "Starting...\n";
    scheduling::WaypointScheduler scheduler;
    auto logSubscriber = std::make_shared<LogWaypointScheduleSubscriber>();

    std::cout << "Adding subscriber\n";
    scheduler.addSubscriber(logSubscriber->shared_from_this());

    std::cout << "Starting waypoint scheduler\n";
    scheduler.start();

    sleep(120);

    std::cout << "Stopping waypoint scheduler\n";
    scheduler.stop();

    return 0;
}
