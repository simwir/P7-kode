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

int main(int argc, char *argv[])
{
    std::cout << "Starting...\n";

    std::filesystem::path model_path, query_path;
    const auto working_path = std::filesystem::current_path();
    const auto bin_loc = std::filesystem::path{argv[0]}.remove_filename();

    if (argc == 3) {
        model_path = std::filesystem::path{argv[1]};
        query_path = std::filesystem::path{argv[2]};
    }
    else {
        model_path = working_path / bin_loc / "waypoint_scheduling.xml";
        query_path = working_path / bin_loc / "waypoint_scheduling.q";
    }

    scheduling::StationScheduler scheduler{model_path, query_path};
    auto logSubscriber = std::make_shared<LogStationScheduleSubscriber>();

    // Stations
    scheduling::StationScheduler stationScheduler;
    auto logStationSubscriber = std::make_shared<LogStationScheduleSubscriber>();

    std::cout << "main: \tAdding station subscriber\n";
    stationScheduler.add_subscriber(logStationSubscriber->shared_from_this());

    std::cout << "main: \tStarting station scheduler\n";
    stationScheduler.start();

    // Stations
    scheduling::WaypointScheduler waypointScheduler;
    auto logWaypointSubscriber = std::make_shared<LogWaypointScheduleSubscriber>();

    std::cout << "main: \tAdding waypoint subscriber\n";
    waypointScheduler.add_subscriber(logWaypointSubscriber->shared_from_this());

    std::cout << "main: \tStarting waypoint scheduler\n";
    waypointScheduler.start();

    scheduling::EtaExtractor eta_extractor;
    auto eta_logger = std::make_shared<LogEtaSubscriber>();
    eta_extractor.add_subscriber(eta_logger->shared_from_this());

    std::cout << "main: \tStarting ETA extractor";
    eta_extractor.start();

    //sleep(120);

    std::cout << "main: \tStopping schedulers\n";
    stationScheduler.wait_for_result();
    waypointScheduler.wait_for_result();
    eta_extractor.wait_for_result();

    std::cout << "main: \treturning from main" << std::endl;
    return 0;
}
