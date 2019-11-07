#ifndef WAYPOINT_SCHEDULER_HPP
#define WAYPOINT_SCHEDULER_HPP

#include "scheduler.hpp"
#include "uppaal_executor.hpp"
#include "uppaal_simulation_parser.hpp"
#include <memory>
#include <queue>
#include <thread>
#include <vector>

namespace scheduling {

enum class ActionType { Hold, Waypoint };

struct Action {
    Action(ActionType type, int value) : type(type), value(value) {}

    ActionType type;
    int value;
};

class WaypointScheduleSubscriber : public std::enable_shared_from_this<WaypointScheduleSubscriber> {
  public:
    virtual void newSchedule(const std::vector<Action> &schedule) = 0;
    virtual ~WaypointScheduleSubscriber() {}
};

class WaypointScheduler : public Scheduler<WaypointScheduleSubscriber, std::vector<Action>> {
  public:
    WaypointScheduler() : Scheduler("waypoint_scheduling.xml", "waypoint_scheduling.q") {}
    void start() override;

  private:
    void run();
    std::vector<scheduling::Action>
    convertResult(const std::vector<scheduling::SimulationExpression> &values);
    void notify_subscribers(const std::vector<Action> &) override;

    std::thread worker;
    UppaalSimulationParser parser;
};

} // namespace scheduling

#endif // WAYPOINT_SCHEDULER_HPP
