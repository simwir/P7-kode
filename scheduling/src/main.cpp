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

    // Stations
    scheduling::StationScheduler stationScheduler;
    auto logStationSubscriber = std::make_shared<LogStationScheduleSubscriber>();

    std::cout << "main: \tAdding station subscriber\n";
    stationScheduler.add_subscriber(logStationSubscriber->shared_from_this());

    std::cout << "main: \tStarting station scheduler\n";
    stationScheduler.start();

    // Waypoints
    scheduling::WaypointScheduler waypointScheduler;
    auto logWaypointSubscriber = std::make_shared<LogWaypointScheduleSubscriber>();

    std::cout << "main: \tAdding waypoint subscriber\n";
    waypointScheduler.add_subscriber(logWaypointSubscriber->shared_from_this());

    std::cout << "main: \tStarting waypoint scheduler\n";
    waypointScheduler.start();

    scheduling::EtaExtractor etaExtractor;
    auto etaLogger = std::make_shared<LogEtaSubscriber>();
    etaExtractor.add_subscriber(etaLogger->shared_from_this());

    std::cout << "main: \tStarting ETA extractor";
    etaExtractor.start();

    // sleep(120);

    std::cout << "main: \tStopping schedulers\n";
    stationScheduler.wait_for_result();
    waypointScheduler.wait_for_result();
    etaExtractor.wait_for_result();

    std::cout << "main: \treturning from main" << std::endl;
    return 0;
}
