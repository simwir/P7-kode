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

    auto callback = [&](const std::string &result) {
        std::cerr << "StationScheduler: Parsing..." << std::endl;
        std::vector<SimulationExpression> values = parser.parse(result, 2);

        std::cerr << "StationScheduler: Composing..." << std::endl;
        std::vector<int> schedule = convertResult(values);

        std::cerr << "StationScheduler: Emitting..." << std::endl;
        notify_subscribers(schedule);
    };
    std::cerr << "StationScheduler: Executing..." << std::endl;
    executor.execute(callback);
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
