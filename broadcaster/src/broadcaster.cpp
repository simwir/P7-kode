#include <bits/socket.h>
#include <functional>
#include "include/broadcaster.hpp"
#include "include/server.hpp"
#include <tcp/exceptions/exceptions.hpp>
#include <tcp/receive.hpp>
#include <tcp/utility/split.hpp>
#include <tcp/send.hpp>


#include <thread>
#include <mutex>

std::mutex mutex;

std::vector<std::string> split_message(const std::string message){
    std::string start = message.substr(0, message.find(','));
    std::string end = message.substr(message.find(',') + 1);
    return std::vector<std::string> {start, end};
}

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

void post_robot_location(Json::Val) {
    mutex.lock();
    data.location_map[robot_id] = new_loc;
    mutex.unlock();
}
void get_robot_locations(std::vector<int> fds) {
}

void post_robot_location(robot_data &data, location &new_loc, int robot_id) {
    mutex.lock();
    data.location_map[robot_id] = new_loc;
    mutex.unlock();
}

void callFunction(Functions functions, Json::Value, robot_data data) {
    try {
        switch (functions) {
            case Functions::post_robot_location:
                post_robot_location()
        }
    }
}

void parseMessage(int fd, robot_data data) {
    std::vector<std::string> result;
    try {
        auto messages = tcp::receive(fd, MSG_DONTWAIT);
        if (!messages.empty()){
            for (const std::string &message : messages) {
                result = split_message(message);
                Functions function = parse_function(result[0]);
                callFunction(function, Json::Value(result[1]), data); //Update callFunc
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


