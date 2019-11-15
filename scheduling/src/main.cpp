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
#include "spdlog/spdlog.h"

class LogWaypointScheduleSubscriber : public scheduling::WaypointScheduleSubscriber {
    void newSchedule(const std::vector<scheduling::Action> &schedule) override
    {
        spdlog::info("Got new waypoint schedule");

        for (auto action : schedule) {
            std::string type = "Unknown";
            if (action.type == scheduling::ActionType::Hold) {
                type = "Hold";
            }
            else if (action.type == scheduling::ActionType::Waypoint) {
                type = "Waypoint";
            }
            spdlog::info("Action: {}, Value: {}", type, action.value);
        }
    }
};

class LogStationScheduleSubscriber : public scheduling::StationScheduleSubscriber {
    void newSchedule(const std::vector<int> &schedule) override
    {
        spdlog::info("Got new station schedule");

        std::stringstream ss;
        ss << "Stations: ";
        for (auto station : schedule) {
            ss << station << " ";
        }

        spdlog::info(ss.str());
    }
};

class LogEtaSubscriber : public scheduling::EtaSubscriber {
    void new_eta(const double eta) override { spdlog::info("Eta found: {}", eta); }
};

int main(int argc, char *argv[])
{
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
    spdlog::info("Starting...");

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

    scheduling::WaypointScheduler scheduler{model_path, query_path};
    auto logSubscriber = std::make_shared<LogWaypointScheduleSubscriber>();

    // Waypoints
    scheduling::WaypointScheduler waypointScheduler;
    auto logWaypointSubscriber = std::make_shared<LogWaypointScheduleSubscriber>();

    spdlog::info("Adding waypoint subscriber");
    waypointScheduler.addSubscriber(logWaypointSubscriber->shared_from_this());

    spdlog::info("Starting waypoint scheduler");
    waypointScheduler.start();

    // Stations
    scheduling::StationScheduler stationScheduler;
    auto logStationSubscriber = std::make_shared<LogStationScheduleSubscriber>();

    spdlog::info("Adding station subscriber");
    stationScheduler.addSubscriber(logStationSubscriber->shared_from_this());

    spdlog::info("Starting station scheduler");
    stationScheduler.start();

    /*scheduling::EtaExtractor eta_extractor;
    auto eta_logger = std::make_shared<LogEtaSubscriber>();
    eta_extractor.addSubscriber(eta_logger->shared_from_this());

    spdlog::info("Starting ETA extractor");
    eta_extractor.start();*/

    sleep(120);

    spdlog::info("Stopping schedulers");
    waypointScheduler.wait_for_schedule();
    stationScheduler.wait_for_schedule();
    //eta_extractor.wait_for_eta();

    return 0;
}
