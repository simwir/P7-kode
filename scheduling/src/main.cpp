// POSIX includes
#include <unistd.h>

// Other includes
#include <iostream>
#include <memory>

#include "eta_extractor.hpp"
#include "station_scheduler.hpp"
#include "waypoint_scheduler.hpp"

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

class LogStationScheduleSubscriber : public scheduling::StationScheduleSubscriber {
    void newSchedule(const std::vector<int> &schedule) override
    {
        std::time_t result = std::time(nullptr);
        std::cout << "Got new station schedule at " << std::asctime(std::localtime(&result));

        std::cout << "Stations: ";
        for (auto station : schedule) {
            std::cout << station << " ";
        }

        std::cout << std::endl;
    }
};

class LogEtaSubscriber : public scheduling::EtaSubscriber {
    void new_eta(const double eta) override { std::cout << "Eta found: " << eta << std::endl; }
};

int main()
{
    std::cout << "Starting...\n";

    // Waypoints
    scheduling::WaypointScheduler waypointScheduler;
    auto logWaypointSubscriber = std::make_shared<LogWaypointScheduleSubscriber>();

    std::cout << "Adding waypoint subscriber\n";
    waypointScheduler.addSubscriber(logWaypointSubscriber->shared_from_this());

    std::cout << "Starting waypoint scheduler\n";
    waypointScheduler.start();

    // Stations
    scheduling::StationScheduler stationScheduler;
    auto logStationSubscriber = std::make_shared<LogStationScheduleSubscriber>();

    std::cout << "Adding station subscriber\n";
    stationScheduler.addSubscriber(logStationSubscriber->shared_from_this());

    std::cout << "Starting station scheduler\n";
    stationScheduler.start();

    scheduling::EtaExtractor eta_extractor;
    auto eta_logger = std::make_shared<LogEtaSubscriber>();
    eta_extractor.addSubscriber(eta_logger->shared_from_this());

    std::cout << "Starting ETA extractor";
    eta_extractor.start();

    sleep(120);

    std::cout << "Stopping schedulers\n";
    waypointScheduler.wait_for_schedule();
    stationScheduler.wait_for_schedule();
    eta_extractor.wait_for_eta();

    return 0;
}
