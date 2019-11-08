#ifndef MASTER_SUBSCRIBER_HPP
#define MASTER_SUBSCRIBER_HPP

#include "eta_extractor.hpp"
#include "station_scheduler.hpp"
#include "util/pollable.hpp"
#include "waypoint_scheduler.hpp"

namespace robot {
class AsyncStationSubscriber : public scheduling::StationScheduleSubscriber,
                               public Pollable<std::vector<int>> {
    void newSchedule(const std::vector<int> &schedule) override { reset(schedule); }
};

class AsyncWaypointSubscriber : public scheduling::WaypointScheduleSubscriber,
                                public Pollable<std::vector<scheduling::Action>> {
    void newSchedule(const std::vector<scheduling::Action> &schedule) override { reset(schedule); }
};

class AsyncEtaSubscriber : public scheduling::EtaSubscriber, public Pollable<double> {
    void new_eta(const double eta) override { reset(eta); }
};

} // namespace robot

#endif
