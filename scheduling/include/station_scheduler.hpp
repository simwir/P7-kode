#ifndef STATION_SCHEDULER_HPP
#define STATION_SCHEDULER_HPP

#include <thread>
#include <vector>
#include <queue>
#include <memory>
#include "uppaal_executor.hpp"
#include "uppaal_simulation_parser.hpp"

namespace scheduling {

    class StationScheduleSubscriber : public std::enable_shared_from_this<StationScheduleSubscriber> {
public:
    virtual void newSchedule(const std::vector<int>& schedule) = 0;
    virtual ~StationScheduleSubscriber() { }
};

class StationScheduler {
public:
    StationScheduler() : executor("station_scheduling.xml", "station_scheduling.q") { }
    void start();
    void stop();
    void addSubscriber(std::shared_ptr<StationScheduleSubscriber> subscriber);

private:
    void run();
    std::vector<int> convertResult(const std::vector<scheduling::SimulationValue>& values);
    void emitSchedule(const std::vector<int>& schedule);

    std::thread worker;
    std::vector<std::weak_ptr<StationScheduleSubscriber>> subscribers;
    bool shouldStop;

    UppaalExecutor executor;
    UppaalSimulationParser parser;
};

}


#endif // STATION_SCHEDULER_HPP
