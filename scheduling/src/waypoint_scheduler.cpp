/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler
 *Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 *associated documentation files (the "Software"), to deal in the Software without restriction,
 *including without limitation the rights to use, copy, modify, merge, publish, distribute,
 *sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or
 *substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 *NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
 *OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <algorithm>
#include <cassert>
#include <exception>
#include <iostream>
#include <queue>
#include <string>
#include <utility>

#include <waypoint_scheduler.hpp>

#define TRACEME
#include "trace.def"

extern int errno;

Json::Value scheduling::Action::to_json() const
{
    Json::Value json;

    json["type"] = type == scheduling::ActionType::Hold ? "Hold" : "Waypoint";
    json["value"] = value;

    return json;
}

scheduling::Action scheduling::Action::from_json(const std::string &json)
{
    std::stringstream ss(json);
    Json::Value root;
    ss >> root;

    return scheduling::Action::from_json(root);
}

scheduling::Action scheduling::Action::from_json(const Json::Value &json)
{
    std::string type_str = json["type"].asString();

    if (!(type_str.compare("Hold") == 0 || type_str.compare("Waypoint") == 0)) {
        throw JsonConversionException{};
    }

    scheduling::ActionType type =
        type_str == "Hold" ? scheduling::ActionType::Hold : scheduling::ActionType::Waypoint;
    int value = json["value"].asInt();

    return scheduling::Action{type, value};
}

void scheduling::WaypointScheduler::start_worker()
{
    std::cerr << "WaypointScheduler: Starting a new waypoint scheduling." << std::endl;

    std::cerr << "WaypointScheduler: Executing..." << std::endl;
    executor.execute([&](const std::string &result) {
        std::cerr << "WaypointScheduler: Parsing..." << std::endl;
        // The second formula contains the simulated trace.
        std::vector<scheduling::SimulationExpression> values = parser.parse(result, 1);

        std::cerr << "WaypointScheduler: Composing..." << std::endl;
        std::vector<scheduling::Action> schedule = convert_result(values);

        TRACE({
            std::cerr << "WaypointScheduler: Schedule is ";
            for (auto i : schedule) {
                std::cerr << i << " ";
            }
        });
        std::cerr << "WaypointScheduler: Emitting..." << std::endl;
        notify_subscribers(schedule);
    });
}

std::vector<scheduling::Action> scheduling::WaypointScheduler::convert_result(
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
    dest_waypoint.pop();
    scheduling::TimeValuePair last_dest = dest_waypoint.front();
    assert(dest_waypoint.front().value == -1);
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
        schedule.push_back(scheduling::Action{scheduling::ActionType::Waypoint, last_dest.value});

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
            schedule.push_back(scheduling::Action{scheduling::ActionType::Hold, delay});
        }
    }

    return schedule;
}

void scheduling::WaypointScheduler::notify_subscribers(
    const std::vector<scheduling::Action> &schedule)
{
    for (auto subscriber : subscribers) {
        if (auto sub = subscriber.lock()) {
            sub->new_schedule(schedule);
        }
    }
}

std::ostream &scheduling::operator<<(std::ostream &os, const scheduling::Action &action)
{
    return os << (action.type == scheduling::ActionType::Hold ? "hold" : "wayp") << ' '
              << action.value;
}
