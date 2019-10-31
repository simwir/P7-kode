#include <port_discovery/port_service.hpp>
#include <tcp/connection.hpp>

#include <assert.h>
#include <iostream>
#include <map>
#include <mutex>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

std::mutex robot_map_lock;

std::vector<std::string> split(const std::string &input, char delimiter)
{
    std::vector<std::string> result;
    size_t current, previous = 0;
    current = input.find(delimiter);

    while (current != std::string::npos) {
        result.push_back(input.substr(previous, current - previous));
        previous = current + 1;
        current = input.find(delimiter, previous);
    }
    result.push_back(input.substr(previous, std::string::npos));

    return result;
}

port_discovery::Function parse_function(const std::string &function)
{
    if (function == "add_robot") {
        return port_discovery::Function::add_robot;
    }
    else if (function == "get_robot") {
        return port_discovery::Function::get_robot;
    }
    else if (function == "remove_robot") {
        return port_discovery::Function::remove_robot;
    }
    else {
        throw port_discovery::UnreadableFunctionException(function);
    }
}

void add_robot(int id, int port, std::map<const int, int> &robot_map)
{
    auto search = robot_map.find(id);
    if (search == robot_map.end()) {
        std::scoped_lock l{robot_map_lock};
        robot_map.insert(std::make_pair(id, port));
        std::cout << "Robot added with id: " << id << " and port: " << port << std::endl;
    }
    else {
        throw port_discovery::IdAlreadyDefinedException(std::to_string(id));
    }
}

void get_robot(int id, const std::map<const int, int> &robot_map, std::shared_ptr<tcp::Connection> connection)
{
    try {
        const std::string message = std::to_string(robot_map.at(id));
        connection->send(message);
    }
    catch (std::out_of_range &e) {
        connection->send("No robot with Id " + std::to_string(id));
    }
}

void remove_robot(int id, std::map<const int, int> &robot_map)
{
    std::scoped_lock l{robot_map_lock};
    robot_map.erase(id);
}

void call_function(port_discovery::Function function, const std::vector<std::string> &parameters,
                   std::map<const int, int> &robot_map, std::shared_ptr<tcp::Connection> connection)
{
    try {
        switch (function) {
        case port_discovery::Function::get_robot:
            if (parameters.size() == 2) {
                const auto id = stoi(parameters[1]);
                get_robot(id, robot_map, connection);
            }
            else {
                throw port_discovery::InvalidParametersException(std::to_string(parameters.size()));
            }
            break;
        case port_discovery::Function::add_robot:
            if (parameters.size() == 3) {
                const auto id = stoi(parameters[1]);
                const auto port = stoi(parameters[2]);
                add_robot(id, port, robot_map);
            }
            else {
                throw port_discovery::InvalidParametersException(std::to_string(parameters.size()));
            }
            break;
        case port_discovery::Function::remove_robot:
            if (parameters.size() == 2) {
                const auto id = stoi(parameters[1]);
                remove_robot(id, robot_map);
            }
            else {
                throw port_discovery::InvalidParametersException(std::to_string(parameters.size()));
            }
            break;
        }
    }
    catch (const std::invalid_argument &e) {
        std::string message = "Invalid argument";
        connection->send(message + e.what());
    }
    catch (const port_discovery::IdAlreadyDefinedException &e) {
        std::string message = "Tried to add robot, but the id was already in the map. Id: ";
        connection->send(message + e.what());
    }
}

void parse_message(std::shared_ptr<tcp::Connection> connection, std::map<const int, int> &robot_map)
{
    std::vector<std::string> args;
    try {
        auto messages = connection->receive(MSG_DONTWAIT);
        for (const std::string &message : messages) {
            std::cout << message << "\n" << std::endl;
            args = split(message, ',');
            port_discovery::Function function = parse_function(args[0]);
            call_function(function, args, robot_map, connection);
        }
    }
    catch (tcp::MalformedMessageException &e) {
        connection->send(e.what());
    }
    catch (port_discovery::UnreadableFunctionException &e) {
        connection->send(e.what());
    }
    catch (port_discovery::InvalidParametersException &e) {
        connection->send(e.what());
    }
}

void port_discovery::PortService::start()
{
    std::cout << "Waiting for connections on port: " << server.get_port() << std::endl;
    while (true) {
        try {
            std::shared_ptr<tcp::Connection> connection = server.accept();
            std::thread thread{parse_message, connection, std::ref(robot_map)};
            thread.detach();
        }
        catch (tcp::AcceptException &e) {
            std::cerr << e.what() << "\n";
        }
    }
}