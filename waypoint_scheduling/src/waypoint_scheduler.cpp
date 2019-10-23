constexpr int PARENT_READ = 0;
constexpr int CHILD_WRITE = 1;
constexpr int CHILD_READ = 2;
constexpr int PARENT_WRITE = 3;
constexpr int NO_FLAGS = 0;

// POSIX includes
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

// Other includes
#include <string>
#include <iostream>
#include <exception>
#include <sstream>
#include <regex>
#include <queue>
#include <utility>

#include <waypoint_scheduler.hpp>

extern int errno;

void waypoint_scheduling::WaypointScheduler::start() {
    shouldStop = false;
    worker = std::thread(&WaypointScheduler::run, this);
}

void waypoint_scheduling::WaypointScheduler::stop() {
    shouldStop = true;
    worker.join();
}

void waypoint_scheduling::WaypointScheduler::addSubscriber(waypoint_scheduling::WaypointScheduleSubscriber& subscriber) {
    subscribers.push_back(&subscriber);
}

void waypoint_scheduling::WaypointScheduler::run() {
    while(true) {
        if (shouldStop) {
            break;
        }

        std::cout << "Starting new waypoint scheduling\n";

        pid_t pid;
        int fd[4];

        pipe(fd);
        pipe(fd + 2);

        pid = fork();

        if (pid == 0) {
            // Child
            dup2(fd[CHILD_WRITE], STDOUT_FILENO);
            dup2(fd[CHILD_READ], STDIN_FILENO);
            close(fd[PARENT_WRITE]);
            close(fd[PARENT_READ]);

            const char* command = "verifyta";
            const char* model = "waypoint_scheduling.xml";
            const char* queries = "waypoint_scheduling.q";

            int ret = execlp(command, command, model, queries, nullptr);

            if (ret == -1) {
                std::cout << "Could not start verifyta. errno: " << errno << ".\n";
                throw WaypointSchedulingException();
            }
        }
        else {
            // Parent
            close(fd[CHILD_WRITE]);
            close(fd[CHILD_READ]);

            // Wait for completion
            std::cout << "Waiting for completion\n";
            int status;
            waitpid(pid, &status, NO_FLAGS);
            std::cout << "Scheduling complete with status " << status << "\n";

            // Only do something if we actually did get a result
            if (status == 0) {
                std::vector<waypoint_scheduling::Action> result = parseResult(fd[PARENT_READ]);
                emitSchedule(result);
            }
            else if (status == 134) { // SIGABORT
                std::cout << "Could not start verifyta\n";
                break;
            }

            // Cleanup after use
            close(fd[PARENT_WRITE]);
            close(fd[PARENT_READ]);
        }
    }
}

std::vector<waypoint_scheduling::Action> waypoint_scheduling::WaypointScheduler::parseResult(int fd) {
    std::cout << "Reading..." << std::endl;
    // Read all from pipe
    std::stringstream ss;
    char buffer[257];
    ssize_t bytes = 0;
    
    while ((bytes = read(fd, buffer, 256)) > 0) {
        if (0 < bytes && bytes <= 256) {
            buffer[bytes] = '\0';
        }
        buffer[256] = '\0';
        
        ss << buffer;
    }
    
    std::cout << "Parsing..." << std::endl;
    // Parse into queues
    std::string result = ss.str();
    std::regex r("\\d+");
    
    std::queue<std::pair<double, int>> cur_waypoint;
    std::queue<std::pair<double, int>> dest_waypoint;
    std::queue<std::pair<double, int>> hold;
    
    std::size_t index = result.find("[0]");
    for (int i = 0; i < 3 && index != std::string::npos; i++) {
        if (index == std::string::npos) {
            throw StrategyParseException("Could not find [0]");
        }
        
        // Find end of line
        std::size_t indexNewline = result.find("\n", index);
        if (indexNewline == std::string::npos) {
            throw StrategyParseException("Could not find EOL");
        }
        
        std::string line = result.substr(index + 4, indexNewline - index - 4); // +4 because "[0]:"
        
        // Find all (t, n) pairs
        int state = 0;
        std::stringstream time;
        std::stringstream value;
        
        for(char& c : line) {
            if (isspace(c)) {
                continue;
            }
            
            switch (state) {
                case 0:
                    if (c != '(') {
                        throw StrategyParseException("State 0 missing (");
                    }
                    state = 1;
                    break;
                case 1:
                    if (isdigit(c)) {
                        time << c;
                    }
                    else if (c == '.') {
                        time << c;
                        state = 2;
                    }
                    else if (c == ',') {
                        state = 4;
                    }
                    else {
                        throw StrategyParseException("State 1 missing digit, . or ,");
                    }
                    break;
                case 2:
                    if (isdigit(c)) {
                        time << c;
                        state = 3;
                    }
                    else {
                        throw StrategyParseException("State 2 missing digit");
                    }
                    break;
                case 3:
                    if (isdigit(c)) {
                        time << c;
                    }
                    else if (c == ',') {
                        state = 4;
                    }
                    else {
                        throw StrategyParseException("State 3 missing digit or ,");
                    }
                    break;
                case 4:
                    if (isdigit(c)) {
                        value << c;
                        state = 5;
                    }
                    else {
                        throw StrategyParseException("State 4 missing digit");
                    }
                    break;
                case 5:
                    if (isdigit(c)) {
                        value << c;
                    }
                    else if (c == ')') {
                        double time_double = std::stod(time.str());
                        int value_int = std::stoi(value.str());
                        // Clear string streams
                        time.str(std::string());
                        value.str(std::string());
                        
                        switch (i) {
                            case 0:
                                cur_waypoint.push(std::make_pair(time_double, value_int));
                                break;
                            case 1:
                                dest_waypoint.push(std::make_pair(time_double, value_int));
                                break;
                            case 2:
                                hold.push(std::make_pair(time_double, value_int));
                                break;
                        }
                        state = 0;
                    }
                    else {
                        throw StrategyParseException("State 5 missing digit or )");
                    }
                    break;
            }
        }
        
        index = result.find("[0]", index + 1);
    }
    
    
    std::cout << "Composing..."  << std::endl;
    // Convert queues to schedules
    std::vector<waypoint_scheduling::Action> schedule;
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
        schedule.push_back(waypoint_scheduling::Action(waypoint_scheduling::ActionType::Waypoint, last_dest.second));
                
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
            schedule.push_back(waypoint_scheduling::Action(waypoint_scheduling::ActionType::Hold, delay));
        }
    }
    

    return schedule;
}

void waypoint_scheduling::WaypointScheduler::emitSchedule(const std::vector<waypoint_scheduling::Action>& schedule) {
    for (auto subscriber : subscribers) {
        subscriber->newSchedule(schedule);
    }
}
