#include <broadcaster/broadcaster.hpp>
#include <tcp/server.hpp>
#include <tcp/exception.hpp>
#include <tcp/connection.hpp>

#include <thread>
#include <mutex>
#include <map>

std::mutex mutex;

std::vector<std::string> split_message(const std::string message){
    std::string start = message.substr(0, message.find(','));
    std::string end = message.substr(message.find(',') + 1);
    return std::vector<std::string> {start, end};
}

broadcaster::Functions parse_function(const std::string &function) {
    if (function == "get_robot_locations") {
        return broadcaster::Functions::get_robot_locations;
    } else if (function == "post_robot_location") {
        return broadcaster::Functions::post_robot_location;
    } else {
        throw tcp::MessageException(function);
    }
}
void broadcaster::Broadcaster::get_robot_locations(std::shared_ptr<tcp::Connection> conn) {
    mutex.lock();
    conn->send(location_map.parse_location_map());
    mutex.unlock();
}

void broadcaster::Broadcaster::post_robot_location(Json::Value value) {
    mutex.lock();
    mutex.unlock();
}

void callFunction(broadcaster::Functions functions, Json::Value value) {
    try {
        switch (functions) {
            case broadcaster::Functions::post_robot_location:
                broadcaster::post_robot_location(value)
                break;
            case broadcaster::Functions::get_robot_location:
                broadcaster::get_robot_location();
                break;
        }
    }
}

void broadcaster::Broadcaster::parseMessage(std::shared_ptr<tcp::Connection> conn) {
    std::vector<std::string> result;
    try {
        auto messages = conn->receive();
        if (!messages.empty()){
            for (const std::string &message : messages) {
                std::vector<std::string> result = split_message(message);
                broadcaster::Functions function = parse_function(result[0]);
                callFunction(function, result[1], robot_info);
            }
        }
    } catch (tcp::MalformedMessageException &e) {
        conn->send(e.what());
    } catch (port_discovery::UnreadableFunctionException &e) {
        conn->send(e.what());
    } catch (port_discovery::InvalidParametersException &e) {
        conn->send(e.what());
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


