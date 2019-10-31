#include <bits/socket.h>
#include <functional>
#include "../include/broadcaster.hpp"
#include "include/server.hpp"
#include <tcp/exceptions/exceptions.hpp>
#include <tcp/receive.hpp>
#include <tcp/utility/split.hpp>
#include <tcp/include/connection.hpp>

#include <thread>
#include <mutex>
#include <map>

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
void broadcaster::Broadcaster::get_robot_locations(std::shared_ptr<tcp::Connection> conn) {
    mutex.lock();
    conn.send(location_map.parse_location_map());
    mutex.unlock();
}

void broadcaster::Broadcaster::post_robot_location(Json::Value value) {
    mutex.lock();
    location_map[robot_id] = new_loc;
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

void broadcaster::Broadcaster::parseMessage(std::shared_ptr<tcp::Connection> conn) {
    std::vector<std::string> result;
    try {
        auto messages = conn->receive();
        if (!messages.empty()){
            for (const std::string &message : messages) {
                result = split_message(message);
                get_robot_locations(conn)
                Functions function = parse_function(result[0]);
                callFunction(function, Json::Value(result[1]), robot_info); //Update callFunc
            }
        }
    } catch (tcp::MalformedMessageException &e) {
        conn->send(e.what());
    } catch (tcp::UnreadableFunctionException &e) {
        tcp::send(fd, e.what());
    } catch (tcp::InvalidParametersException &e) {
        tcp::send(fd, e.what());
    }
}

broadcaster::Broadcaster(int port) : server(port) {}

void broadcaster::Broadcaster::start_broadcasting() {
    std::vector()
    while (true) {
        std::shared_ptr<tcp::Connection> conn = server.accept();
        std::thread t1(&Broadcaster::parseMessage, this, conn);
    }
}


