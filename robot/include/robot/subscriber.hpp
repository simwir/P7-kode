#ifndef MASTER_SUBSCRIBER_HPP
#define MASTER_SUBSCRIBER_HPP

#include "eta_extractor.hpp"
#include "station_scheduler.hpp"
#include "util/pollable.hpp"
#include "waypoint_scheduler.hpp"

#include <iostream>
#include <mutex>
#include <vector>

namespace robot {
class AsyncStationSubscriber : public scheduling::StationScheduleSubscriber,
                               public Pollable<std::vector<int>> {
    std::mutex mutex;

  public:
    AsyncStationSubscriber(std::vector<int> station_ids) : station_ids(station_ids) {}

  private:
    const std::vector<int> station_ids;

    void newSchedule(const std::vector<int> &schedule) override
    {
        std::scoped_lock _{mutex};
        reset(schedule);
    }
};

class AsyncWaypointSubscriber : public scheduling::WaypointScheduleSubscriber,
                                public Pollable<std::vector<scheduling::Action>> {
  public:
    AsyncWaypointSubscriber(std::vector<int> waypoint_ids) : waypoint_ids(waypoint_ids) {}

  private:
    std::mutex mutex;
    void newSchedule(const std::vector<scheduling::Action> &schedule) override
    {
        std::scoped_lock _{mutex};
        reset(schedule);
    }
    const std::vector<int> waypoint_ids;
};

class AsyncEtaSubscriber : public scheduling::EtaSubscriber, public Pollable<double> {
    void new_eta(const double eta) override { reset(eta); }
};

} // namespace robot

#endif
