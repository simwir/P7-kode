#include <algorithm>
#include <exception>
#include <iostream>
#include <queue>
#include <string>
#include <utility>

#include <waypoint_scheduler.hpp>

extern int errno;

void scheduling::WaypointScheduler::start()
{
    worker = std::thread(&WaypointScheduler::run, this);
}

void scheduling::WaypointScheduler::wait_for_schedule()
{
    worker.join();
}

void scheduling::WaypointScheduler::addSubscriber(
    std::shared_ptr<scheduling::WaypointScheduleSubscriber> subscriber)
{
    subscribers.push_back(subscriber->weak_from_this());
}

void scheduling::WaypointScheduler::run()
{
    std::cout << "Starting a new waypoint scheduling." << std::endl;

    std::cout << "Executing..." << std::endl;
    std::string result = executor.execute();

    std::cout << "Parsing..." << std::endl;
    std::vector<scheduling::SimulationExpression> values = parser.parse(result, 2);

    std::cout << "Composing..." << std::endl;
    std::vector<scheduling::Action> schedule = convertResult(values);

    std::cout << "Emitting..." << std::endl;
    emitSchedule(schedule);
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

void scheduling::WaypointScheduler::emitSchedule(const std::vector<scheduling::Action> &schedule)
{
    for (auto subscriber : subscribers) {
        if (auto sub = subscriber.lock()) {
            sub->newSchedule(schedule);
        }
    }
}
