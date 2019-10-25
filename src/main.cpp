// POSIX includes
#include <unistd.h>

// Other includes
#include <iostream>

#include <waypoint_scheduler.hpp>
#include <station_scheduler.hpp>

class LogWaypointScheduleSubscriber : public scheduling::WaypointScheduleSubscriber  {
    void newSchedule(const std::vector<scheduling::Action>& schedule) {
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

class LogStationScheduleSubscriber : public scheduling::StationScheduleSubscriber  {
    void newSchedule(const std::vector<int>& schedule) {
        std::time_t result = std::time(nullptr);
        std::cout << "Got new station schedule at " << std::asctime(std::localtime(&result));
        
        std::cout << "Stations: ";
        for (auto station : schedule) {
            std::cout << station << " ";
        }
        
        std::cout << std::endl;
    }
};

int main() {
    std::cout << "Starting...\n";
    
    // Waypoints
    scheduling::WaypointScheduler waypointScheduler;
    LogWaypointScheduleSubscriber logWaypointSubscriber;
    
    std::cout << "Adding waypoint subscriber\n";
    waypointScheduler.addSubscriber(logWaypointSubscriber);
    
    std::cout << "Starting waypoint scheduler\n";
    waypointScheduler.start();
    
    // Stations
    scheduling::StationScheduler stationScheduler;
    LogStationScheduleSubscriber logStationSubscriber;
    
    std::cout << "Adding station subscriber\n";
    stationScheduler.addSubscriber(logStationSubscriber);
    
    std::cout << "Starting station scheduler\n";
    stationScheduler.start();
    
    sleep(120);
    
    std::cout << "Stopping schedulers\n";
    waypointScheduler.stop();
    stationScheduler.stop();

    return 0;
}