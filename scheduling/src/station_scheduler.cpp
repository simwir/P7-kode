#include <algorithm>
#include <deque>
#include <exception>
#include <iostream>
#include <queue>
#include <string>
#include <utility>

#include "station_scheduler.hpp"

extern int errno;
namespace scheduling {
void StationScheduler::start()
{
    shouldStop = false;
    worker = std::thread(&StationScheduler::run, this);
}

void StationScheduler::stop()
{
    shouldStop = true;
    worker.join();
}

void StationScheduler::addSubscriber(std::shared_ptr<StationScheduleSubscriber> subscriber)
{
    subscribers.push_back(subscriber->weak_from_this());
}

void StationScheduler::run()
{
    while (true) {
        if (shouldStop) {
            break;
        }

        std::cout << "Starting a new waypoint scheduling.\n";

        std::cout << "Executing..." << std::endl;
        std::string result = executor.execute();

        std::cout << "Parsing..." << std::endl;
        std::vector<SimulationValue> values = parser.parse(result, 2);

        std::cout << "Composing..." << std::endl;
        std::vector<int> schedule = convertResult(values);

        std::cout << "Emitting..." << std::endl;
        emitSchedule(schedule);
    }
}

std::vector<int>
StationScheduler::convertResult(const std::vector<scheduling::SimulationValue> &values)
{
    // Convert into queues
    std::queue<std::pair<double, int>> cur = parser.findFirstRunAsQueue(values, "Robot.cur_loc");
    std::queue<std::pair<double, int>> dest = parser.findFirstRunAsQueue(values, "Robot.dest");

    // Convert queues to schedules
    std::vector<int> schedule;

    cur.pop();
    schedule.push_back(cur.front().second);

    std::pair<double, int> last_dest = dest.front();
    dest.pop();

    while (!dest.empty()) {
        // Find next waypoint
        while (!dest.empty() && dest.front().second == last_dest.second) {
            dest.pop();
        }

        if (dest.empty()) {
            break;
        }

        last_dest = dest.front();
        dest.pop();
        schedule.push_back(last_dest.second);
    }

    return schedule;
}

void StationScheduler::emitSchedule(const std::vector<int> &schedule)
{
    for (auto subscriber : subscribers) {
        if (auto sub = subscriber.lock()) {
            sub->newSchedule(schedule);
        }
    }
}
} // namespace scheduling
