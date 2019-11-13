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

/*void StationScheduler::wait_for_schedule()
{
if (worker.joinable()) {
worker.join();
}
}*/

void StationScheduler::start_worker()
{
    std::cerr << "StationScheduler: Starting a new station scheduling.\n";

    std::cerr << "StationScheduler: Executing..." << std::endl;
    executor.execute([&](const std::string &result) {
        std::cerr << "StationScheduler: Parsing..." << std::endl;
        std::vector<SimulationExpression> values = parser.parse(result, 2);

        std::cerr << "StationScheduler: Composing..." << std::endl;
        std::vector<int> schedule = convertResult(values);

        std::cerr << "StationScheduler: Emitting..." << std::endl;
        notify_subscribers(schedule);
    });
}

std::vector<int> StationScheduler::convertResult(const std::vector<SimulationExpression> &values)
{
    // Convert into queues
    std::queue<TimeValuePair> cur = parser.findFirstRunAsQueue(values, "Robot.cur_loc");
    std::queue<TimeValuePair> dest = parser.findFirstRunAsQueue(values, "Robot.dest");

    // Convert queues to schedules
    std::vector<int> schedule;

    cur.pop();
    schedule.push_back(cur.front().value);

    TimeValuePair last_dest = dest.front();
    dest.pop();

    while (!dest.empty()) {
        // Find next waypoint
        while (!dest.empty() && dest.front().value == last_dest.value) {
            dest.pop();
        }

        if (dest.empty()) {
            break;
        }

        last_dest = dest.front();
        dest.pop();
        schedule.push_back(last_dest.value);
    }

    return schedule;
}

void StationScheduler::notify_subscribers(const std::vector<int> &schedule)
{
    for (auto subscriber : subscribers) {
        if (auto sub = subscriber.lock()) {
            sub->newSchedule(schedule);
        }
    }
}
} // namespace scheduling
