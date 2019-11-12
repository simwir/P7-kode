#include <algorithm>
#include <exception>
#include <iostream>
#include <queue>
#include <string>
#include <utility>

#include <waypoint_scheduler.hpp>

extern int errno;


void scheduling::WaypointScheduler::run()
{
    std::cerr << "Starting a new waypoint scheduling." << std::endl;

    std::cerr << "Executing..." << std::endl;
    auto result = executor.execute();
    if (!result.has_value()) {
        return;
    }

    std::cerr << "Parsing..." << std::endl;
    std::vector<scheduling::SimulationExpression> values = parser.parse(result.value(), 2);

    std::cerr << "Composing..." << std::endl;
    std::vector<scheduling::Action> schedule = convertResult(values);

    std::cerr << "Emitting..." << std::endl;
    notify_subscribers(schedule);
}

std::vector<scheduling::Action> scheduling::WaypointScheduler::convertResult(
    const std::vector<scheduling::SimulationExpression> &values)
{
    // Convert into queues
    std::queue<scheduling::TimeValuePair> cur_waypoint =
        parser.findFirstRunAsQueue(values, "Robot.cur_waypoint");
    std::queue<scheduling::TimeValuePair> dest_waypoint =
        parser.findFirstRunAsQueue(values, "Robot.dest_waypoint");
    std::queue<scheduling::TimeValuePair> hold =
        parser.findFirstRunAsQueue(values, "Robot.Holding");

    // Convert queues to schedules
    std::vector<scheduling::Action> schedule;
    cur_waypoint.pop();
    scheduling::TimeValuePair last_cur = cur_waypoint.front();
    scheduling::TimeValuePair last_dest = dest_waypoint.front();
    dest_waypoint.pop();
    hold.pop();

    while (!dest_waypoint.empty() && !cur_waypoint.empty()) {
        // Find next waypoint
        while (!dest_waypoint.empty() && dest_waypoint.front().value == last_dest.value) {
            dest_waypoint.pop();
        }

        if (dest_waypoint.empty()) {
            break;
        }

        last_dest = dest_waypoint.front();
        dest_waypoint.pop();
        schedule.push_back(scheduling::Action(scheduling::ActionType::Waypoint, last_dest.value));

        // Check when we reach that waypoint
        do {
            last_cur = cur_waypoint.front();
            cur_waypoint.pop();
        } while (last_cur.value != last_dest.value);

        // Check if we should hold
        int delay = 0;
        while (!hold.empty() && hold.front().time - last_cur.time < 0.0001) {
            hold.pop();
        }

        while (!hold.empty() && hold.front().value == 1) {
            delay++;
            hold.pop();
        }

        if (!hold.empty()) {
            hold.pop();
        }

        if (delay > 0) {
            schedule.push_back(scheduling::Action(scheduling::ActionType::Hold, delay));
        }
    }

    return schedule;
}

void scheduling::WaypointScheduler::notify_subscribers(
    const std::vector<scheduling::Action> &schedule)
{
    for (auto subscriber : subscribers) {
        if (auto sub = subscriber.lock()) {
            sub->newSchedule(schedule);
        }
    }
}
