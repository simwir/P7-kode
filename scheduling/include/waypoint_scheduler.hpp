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
#ifndef WAYPOINT_SCHEDULER_HPP
#define WAYPOINT_SCHEDULER_HPP

#include "scheduler.hpp"
#include "uppaal_executor.hpp"
#include "uppaal_simulation_parser.hpp"
#include "util/json.hpp"
#include "util/logger.hpp"
#include <memory>
#include <queue>
#include <vector>

namespace scheduling {

enum class ActionType { Hold, Waypoint };

struct Action {
    ActionType type;
    int value;

    Json::Value to_json() const;
    static Action from_json(const std::string &json);
    static Action from_json(const Json::Value &json);
};

class WaypointScheduleSubscriber : public std::enable_shared_from_this<WaypointScheduleSubscriber> {
  public:
    virtual void newSchedule(const std::vector<Action> &schedule) = 0;
    virtual ~WaypointScheduleSubscriber() = default;
};

class WaypointScheduler : public Scheduler<WaypointScheduleSubscriber, std::vector<Action>> {
  public:
    WaypointScheduler() : Scheduler("waypoint_scheduling.xml", "waypoint_scheduling.q") {}
    WaypointScheduler(const std::filesystem::path &model_path,
                      const std::filesystem::path &query_path)
        : Scheduler(model_path, query_path)
    {
    }

  private:
    void start_worker() override;

    void run();
    std::vector<scheduling::Action>
    convertResult(const std::vector<scheduling::SimulationExpression> &values);
    void notify_subscribers(const std::vector<Action> &) override;

    UppaalSimulationParser parser;
};

} // namespace scheduling

#endif // WAYPOINT_SCHEDULER_HPP
