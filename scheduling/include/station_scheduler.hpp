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
#ifndef STATION_SCHEDULER_HPP
#define STATION_SCHEDULER_HPP

#include "uppaal_executor.hpp"
#include "uppaal_simulation_parser.hpp"
#include <memory>
#include <queue>
#include <thread>
#include <vector>

namespace scheduling {

class StationScheduleSubscriber : public std::enable_shared_from_this<StationScheduleSubscriber> {
  public:
    virtual void newSchedule(const std::vector<int> &schedule) = 0;
    virtual ~StationScheduleSubscriber() {}
};

class StationScheduler {
  public:
    StationScheduler() : executor("station_scheduling.xml", "station_scheduling.q") {}
    void start();
    void wait_for_schedule();
    void addSubscriber(std::shared_ptr<StationScheduleSubscriber> subscriber);

  private:
    void run();
    std::vector<int> convertResult(const std::vector<scheduling::SimulationExpression> &values);
    void emitSchedule(const std::vector<int> &schedule);

    std::thread worker;
    std::vector<std::weak_ptr<StationScheduleSubscriber>> subscribers;

    UppaalExecutor executor;
    UppaalSimulationParser parser;
};

} // namespace scheduling

#endif // STATION_SCHEDULER_HPP
