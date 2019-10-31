#include <bits/socket.h>
#include <functional>
#include "broadcaster.hpp"
#include "server.hpp"
#include "exceptions/exceptions.hpp"
#include "receive.hpp"
#include "utility/split.hpp"
#include "send.hpp"


#include <thread>
#include <mutex>

std::mutex mutex;

Functions parse_function(const std::string &function) {
    if (function == "get_robot_locations") {
        return Functions::get_robot_locations;
    } else if (function == "post_robot_location") {
        return Functions::post_robot_location;
    } else {
        throw tcp::UnreadableFunctionException(function);
    }
}
void get_robot_locations(std::vector<int> fd) {
}

void post_robot_location(robot_data &data, location &new_loc, int robot_id) {
    mutex.lock();
    data.location_map[robot_id] = new_loc;
    mutex.unlock();
}
void get_robot_locations(std::vector<int> fds) {
    for (int fd : fds) {
        tcp::send(fd, "test");
    }
}

void post_robot_location(robot_data &data, location &new_loc, int robot_id) {
    mutex.lock();
    data.location_map[robot_id] = new_loc;
    mutex.unlock();
}

void callFunction(Functions functions, const std::vector<std::string>& parameters, robot_data data) {
    try {
        switch (functions) {
            case Functions::get_robot_locations:
                if (parameters.size() == 3){
                    post_robot_location(data, parameters[1], parameters[2]);
                } else {
                    throw tcp::InvalidParametersException(std::to_string(parameters.size()));
                }
        }
    }
}

void parseMessage(int fd, robot_data data) {
    std::vector<std::string> result;
    try {
        auto messages = tcp::receive(fd, MSG_DONTWAIT);
        if (!messages.empty()){
            for (const std::string &message : messages) {
                result = split(message, ',');
                Functions function = parse_function(result[0]);
                callFunction(function, result, data);
            }
        }
    } catch (tcp::MalformedMessageException &e) {
        tcp::send(fd, e.what());
    } catch (tcp::UnreadableFunctionException &e) {
        tcp::send(fd, e.what());
    } catch (tcp::InvalidParametersException &e) {
        tcp::send(fd, e.what());
    }
}

broadcaster::broadcaster(int port) : server(tcp::Server(port)) {}

void broadcaster::start_broadcasting() {
    struct robot_data data;
    while (true) {

        int client_fd = server.accept();
        std::thread t1(parseMessage, client_fd, std::ref(data));
    }
}