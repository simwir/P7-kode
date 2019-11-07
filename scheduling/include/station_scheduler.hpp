#ifndef STATION_SCHEDULER_HPP
#define STATION_SCHEDULER_HPP

#include "scheduler.hpp"
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

class StationScheduler : public Scheduler<StationScheduleSubscriber, std::vector<int>> {
  public:
    StationScheduler() : Scheduler("station_scheduling.xml", "station_scheduling.q") {}
    void start() override;
    void run();

  private:
    void notify_subscribers(const std::vector<int> &) override;
    std::vector<int> convertResult(const std::vector<SimulationExpression> &values);
    UppaalSimulationParser parser;
};
} // namespace scheduling

#endif // STATION_SCHEDULER_HPP
