// POSIX includes
#include <unistd.h>

// Other includes
#include <iostream>

#include <waypoint_scheduler.hpp>

class LogWaypointScheduleSubscriber : public waypoint_scheduling::WaypointScheduleSubscriber  {
    void newSchedule(const std::vector<waypoint_scheduling::Action>& schedule) {
        std::time_t result = std::time(nullptr);
        std::cout << "Got new waypoint schedule at " << std::asctime(std::localtime(&result));
        
        for (auto action : schedule) {
            std::cout << "Action: ";
            if (action.type == waypoint_scheduling::ActionType::Hold) {
                std::cout << "Hold";
            }
            else if (action.type == waypoint_scheduling::ActionType::Waypoint) {
                std::cout << "Waypoint";
            }
            else {
                std::cout << "Unknown";
            }
            std::cout << ", Value: " << action.value << std::endl;
        }
    }
};

int main() {
    std::cout << "Starting...\n";
    waypoint_scheduling::WaypointScheduler scheduler;
    LogWaypointScheduleSubscriber logSubscriber;
    
    
    std::cout << "Adding subscriber\n";
    scheduler.addSubscriber(logSubscriber);
    
    std::cout << "Starting waypoint scheduler\n";
    scheduler.start();
    
    sleep(120);
    
    std::cout << "Stopping waypoint scheduler\n";
    scheduler.stop();
    
    return 0;
}