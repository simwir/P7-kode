#ifndef WAYPOINT_SCHEDULER_HPP
#define WAYPOINT_SCHEDULER_HPP

#include "scheduler.hpp"
#include "uppaal_executor.hpp"
#include "uppaal_simulation_parser.hpp"
#include "util/json.hpp"
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
