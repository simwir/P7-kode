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
void StationScheduler::start()
{
    worker = std::thread(&StationScheduler::run, this);
}

void StationScheduler::wait_for_schedule()
{
    worker.join();
}

void StationScheduler::addSubscriber(std::shared_ptr<StationScheduleSubscriber> subscriber)
{
    subscribers.push_back(subscriber->weak_from_this());
}

void StationScheduler::run()
{
    std::cout << "Starting a new waypoint scheduling.\n";

    std::cout << "Executing..." << std::endl;
    std::string result = executor.execute();

    std::cout << "Parsing..." << std::endl;
    std::vector<SimulationExpression> values = parser.parse(result, 2);

    std::cout << "Composing..." << std::endl;
    std::vector<int> schedule = convertResult(values);

    std::cout << "Emitting..." << std::endl;
    emitSchedule(schedule);
}

std::vector<int> StationScheduler::convertResult(const std::vector<SimulationExpression> &values)
{
    // Convert into queues
    std::queue<TimeValuePair> dest = parser.findFirstRunAsQueue(values, "Robot.converted_dest()");

    // Convert queues to schedules
    std::vector<int> schedule;

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

void StationScheduler::emitSchedule(const std::vector<int> &schedule)
{
    for (auto subscriber : subscribers) {
        if (auto sub = subscriber.lock()) {
            sub->newSchedule(schedule);
        }
    }
}
} // namespace scheduling
