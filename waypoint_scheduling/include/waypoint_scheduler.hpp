#ifndef WAYPOINT_SCHEDULER_HPP
#define WAYPOINT_SCHEDULER_HPP

#include <thread>
#include <vector>

namespace waypoint_scheduling {

enum class ActionType {
    Hold,
    Waypoint
};

struct Action {
    Action(ActionType type, int value) : type(type), value(value) { }

    ActionType type;
    int value;
};

struct WaypointSchedulingException : public std::exception {
     const char* what() const noexcept { return "Could not run scheduling"; }
};

class StrategyParseException : public std::exception{
    std::string message;
public:
    StrategyParseException(const std::string& inmessage){
        message = inmessage;
    }

    const char* what() const noexcept override{
        return message.c_str();
    }
};

class WaypointScheduleSubscriber {
public:
    virtual void newSchedule(const std::vector<Action>& schedule) = 0;
    virtual ~WaypointScheduleSubscriber() { }
};

class WaypointScheduler {
public:
    void start();
    void stop();
    void addSubscriber(WaypointScheduleSubscriber& subscriber);

private:
    void run();
    std::vector<Action> parseResult(int fd);
    void emitSchedule(const std::vector<Action>& schedule);

    std::thread worker;
    std::vector<WaypointScheduleSubscriber*> subscribers;
    bool shouldStop;
};

}


#endif // WAYPOINT_SCHEDULER_HPP
