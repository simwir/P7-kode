#ifndef WAYPOINT_SCHEDULER_HPP
#define WAYPOINT_SCHEDULER_HPP

#include "uppaal_executor.hpp"
#include "uppaal_simulation_parser.hpp"
#include "util/json.hpp"
#include <memory>
#include <queue>
#include <thread>
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

class WaypointScheduler {
  public:
    WaypointScheduler() : executor("waypoint_scheduling.xml", "waypoint_scheduling.q") {}
    WaypointScheduler(const std::filesystem::path &model_path,
                      const std::filesystem::path &query_path)
        : executor(model_path, query_path)
    {
    }
    void start();
    void wait_for_schedule();
    void addSubscriber(std::shared_ptr<WaypointScheduleSubscriber> subscriber);

  private:
    void run();
    std::vector<scheduling::Action>
    convertResult(const std::vector<scheduling::SimulationExpression> &values);
    void emitSchedule(const std::vector<Action> &schedule);

    std::thread worker;
    std::vector<std::weak_ptr<WaypointScheduleSubscriber>> subscribers;

    UppaalExecutor executor;
    UppaalSimulationParser parser;
};

} // namespace scheduling

#endif // WAYPOINT_SCHEDULER_HPP
