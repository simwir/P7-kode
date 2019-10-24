#include <string>
#include <iostream>
#include <exception>
#include <queue>
#include <deque>
#include <utility>
#include <algorithm>

#include <waypoint_scheduler.hpp>

extern int errno;

void scheduling::WaypointScheduler::start() {
    shouldStop = false;
    worker = std::thread(&WaypointScheduler::run, this);
}

void scheduling::WaypointScheduler::stop() {
    shouldStop = true;
    worker.join();
}

void scheduling::WaypointScheduler::addSubscriber(scheduling::WaypointScheduleSubscriber& subscriber) {
    subscribers.push_back(&subscriber);
}

void scheduling::WaypointScheduler::run() {
    while(true) {
        if (shouldStop) {
            break;
        }

        std::cout << "Starting a new waypoint scheduling.\n";

        std::cout << "Executing..." << std::endl;
        std::string result = executor.execute();

        std::cout << "Parsing..." << std::endl;
        std::vector<SimulationValue> values = parser.parse(result, 2);

        std::cout << "Composing..."  << std::endl;
        std::vector<scheduling::Action> schedule = convertResult(values);

        std::cout << "Emitting..."  << std::endl;
        emitSchedule(schedule);
    }
}

std::queue<std::pair<double, int>> scheduling::WaypointScheduler::findFirstRunAsQueue(const std::vector<scheduling::SimulationValue>& values, const std::string& name) {
    auto value = std::find_if(values.begin(), values.end(),
        [&name](const scheduling::SimulationValue& val) {
            return val.name.compare(name) == 0;
        });
        
    if (value == values.end()) {
        throw NameNotFoundException();
    }
        
    scheduling::Run first_run = value->runs.at(0);
    return std::queue<std::pair<double, int>>(std::deque<std::pair<double, int>>(first_run.values.begin(), first_run.values.end()));
}

std::vector<scheduling::Action> scheduling::WaypointScheduler::convertResult(const std::vector<scheduling::SimulationValue>& values) {
    // Convert into queues
    std::queue<std::pair<double, int>> cur_waypoint = findFirstRunAsQueue(values, "Robot.cur_waypoint");
    std::queue<std::pair<double, int>> dest_waypoint = findFirstRunAsQueue(values, "Robot.dest_waypoint");
    std::queue<std::pair<double, int>> hold = findFirstRunAsQueue(values, "Robot.Holding");

    // Convert queues to schedules
    std::vector<scheduling::Action> schedule;
    cur_waypoint.pop();
    std::pair<double, int> last_cur = cur_waypoint.front();
    std::pair<double, int> last_dest = dest_waypoint.front();
    dest_waypoint.pop();
    hold.pop();

    while (!dest_waypoint.empty() && !cur_waypoint.empty()) {
        // Find next waypoint
        while (!dest_waypoint.empty() && dest_waypoint.front().second == last_dest.second) {
            dest_waypoint.pop();
        }

        if (dest_waypoint.empty()) {
            break;
        }

        last_dest = dest_waypoint.front();
        dest_waypoint.pop();
        schedule.push_back(scheduling::Action(scheduling::ActionType::Waypoint, last_dest.second));

        // Check when we reach that waypoint
        do {
            last_cur = cur_waypoint.front();
            cur_waypoint.pop();
        }
        while (last_cur.second != last_dest.second);

        // Check if we should hold
        int delay = 0;
        while (!hold.empty() && hold.front().first - last_cur.first < 0.0001) {
            hold.pop();
        }

        while (!hold.empty() && hold.front().second == 1) {
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

void scheduling::WaypointScheduler::emitSchedule(const std::vector<scheduling::Action>& schedule) {
    for (auto subscriber : subscribers) {
        subscriber->newSchedule(schedule);
    }
}
