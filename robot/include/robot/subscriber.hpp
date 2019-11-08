#ifndef MASTER_SUBSCRIBER_HPP
#define MASTER_SUBSCRIBER_HPP

#include "eta_extractor.hpp"
#include "station_scheduler.hpp"
#include "util/pollable.hpp"
#include "waypoint_scheduler.hpp"

#include <deque>

namespace robot {
class AsyncStationSubscriber : public scheduling::StationScheduleSubscriber,
                               public Pollable<std::deque<int>> {
    void newSchedule(const std::vector<int> &schedule) override {
        std::deque<int> queue{std::begin(schedule), std::end(schedule)};
        reset(queue);
    }
};

class AsyncWaypointSubscriber : public scheduling::WaypointScheduleSubscriber,
                                public Pollable<std::deque<scheduling::Action>> {
    void newSchedule(const std::vector<scheduling::Action> &schedule) override {
        std::deque<scheduling::Action> queue{std::begin(schedule), std::end(schedule)};
        reset(queue);
    }
};

class AsyncEtaSubscriber : public scheduling::EtaSubscriber, public Pollable<double> {
    void new_eta(const double eta) override { reset(eta); }
};

} // namespace robot

#endif
