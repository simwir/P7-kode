#include <broadcaster/broadcaster.hpp>
#include <tcp/server.hpp>
#include <tcp/exception.hpp>
#include <tcp/connection.hpp>

#include <jsoncpp/json/json.h>

#include <iostream>
#include <thread>
#include <mutex>
#include <map>

namespace broadcaster {

std::mutex mutex;

std::vector<std::string> split_message(const std::string& message) {
    std::string start = message.substr(0, message.find(','));
    std::string end = message.substr(message.find(',') + 1);
    return std::vector<std::string> {start, end};
}

Functions Broadcaster::parse_function(const std::string &function) {
    if (function == "get_robot_info") {
        return Functions::get_robot_info;
    } else if (function == "post_robot_location") {
        return Functions::post_robot_location;
    } else {
        throw tcp::MessageException(function);
    }
}
void Broadcaster::get_robot_info(std::shared_ptr<tcp::Connection> conn) {
    Json::Value result = robotsMap.to_json();
    mutex.lock();
    conn->send(result.toStyledString());
    mutex.unlock();
}

void Broadcaster::post_robot_location(const std::string& value) {
    robot::Info info = robot::Info::from_json(value);
    mutex.lock();
    robotsMap[info.id] = info;
    mutex.unlock();
}

void Broadcaster::call_function(Functions functions, std::string& value, std::shared_ptr<tcp::Connection> conn) {
    switch (functions) {
        case Functions::post_robot_location:
            post_robot_location(value);
            break;
        case Functions::get_robot_info:
            get_robot_info(conn);
            break;
        default: 
            throw UnknownFunctionException("Function not implementet");
    } 
}

void Broadcaster::parse_message(std::shared_ptr<tcp::Connection> conn) {
    try {
        auto messages = conn->receive();
        if (!messages.empty()){
            for (const std::string &message : messages) {
                std::vector<std::string> result = split_message(message);
                Functions function = parse_function(result[0]);
                call_function(function, result[1], conn);
            }
        }
    } catch (tcp::MalformedMessageException &e) {
        conn->send(e.what());
    } catch (UnknownFunctionException &e) {
        conn->send(e.what());
    } catch (UnknownParameterException &e) {
        conn->send(e.what());
    }
}

void Broadcaster::start_broadcasting() {
    std::cout << "Waiting for connections on port: " << server.get_port() << std::endl;
    while (true) {
        std::shared_ptr<tcp::Connection> conn = server.accept();
        std::thread t1(&Broadcaster::parse_message, this, conn);
        t1.detach();
    }
}
}
