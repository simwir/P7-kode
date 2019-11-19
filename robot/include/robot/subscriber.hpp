#ifndef MASTER_SUBSCRIBER_HPP
#define MASTER_SUBSCRIBER_HPP

#include "eta_extractor.hpp"
#include "station_scheduler.hpp"
#include "util/pollable.hpp"
#include "waypoint_scheduler.hpp"

#include <deque>
#include <mutex>

namespace robot {
class AsyncStationSubscriber : public scheduling::StationScheduleSubscriber,
                               public Pollable<std::vector<int>> {
    std::mutex mutex;

  public:
    AsyncStationSubscriber(std::vector<int> station_ids) : station_ids(station_ids) {}

  private:
    void newSchedule(const std::vector<int> &schedule) override
    {
        std::scoped_lock _{mutex};
        std::vector<int> _schedule;
        _schedule.resize(schedule.size());
        std::transform(schedule.begin(), schedule.end(), _schedule.begin(),
                       [&](int uppaal_idx) { return station_ids.at(uppaal_idx); });
        reset(std::move(_schedule));
    }
    const std::vector<int> station_ids;
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
        std::vector<scheduling::Action> _schedule;
        _schedule.resize(schedule.size());
        std::transform(schedule.begin(), schedule.end(), _schedule.begin(), [&](auto action) {
            if (action.type == scheduling::ActionType::Waypoint) {
                return scheduling::Action{action.type, waypoint_ids.at(action.value)};
            }
            else {
                return action;
            }
        });
        reset(_schedule);
    }
    const std::vector<int> waypoint_ids;
};

class AsyncEtaSubscriber : public scheduling::EtaSubscriber, public Pollable<double> {
    void new_eta(const double eta) override { reset(eta); }
};

} // namespace robot

#endif
