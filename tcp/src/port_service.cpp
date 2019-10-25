#include <port_service.hpp>
#include <connection.hpp>

#include <assert.h>
#include <iostream>
#include <map>
#include <mutex>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

std::mutex robot_map_lock;

// This function is duplicated from feature/tcp-class
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

Function parse_function(const std::string &function)
{
    if (function == "add_robot") {
        return Function::add_robot;
    }
    else if (function == "get_robot") {
        return Function::get_robot;
    }
    else if (function == "remove_robot") {
        return Function::remove_robot;
    }
    else {
        throw tcp::UnreadableFunctionException(function);
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
        throw IdAlreadyDefinedException(std::to_string(id));
    }
}

void get_robot(int id, std::map<const int, int> &robot_map, int fd)
{
    try {
        const std::string message = std::to_string(robot_map.at(id));
        tcp::send(fd, message);
    }
    catch (std::out_of_range &e) {
        tcp::send(fd, "No robot with Id " + std::to_string(id));
    }
}

void remove_robot(int id, std::map<const int, int> &robot_map)
{
    std::scoped_lock l{robot_map_lock};
    robot_map.erase(id);
}

void callFunction(Function function, const std::vector<std::string> &parameters,
                  std::map<const int, int> &robotMap, tcp::Connection* connection)
{
    try {
        switch (function) {
        case Function::get_robot:
            if (parameters.size() == 2) {
                const auto id = stoi(parameters[1]);
                get_robot(id, robotMap, client_fd);
            }
            else {
                throw tcp::InvalidParametersException(std::to_string(parameters.size()));
            }
            break;
        case Function::add_robot:
            if (parameters.size() == 3) {
                const auto id = stoi(parameters[1]);
                const auto port = stoi(parameters[2]);
                add_robot(id, port, robotMap);
            }
            else {
                throw tcp::InvalidParametersException(std::to_string(parameters.size()));
            }
            break;
        case Function::remove_robot:
            if (parameters.size() == 2) {
                const auto id = stoi(parameters[1]);
                remove_robot(id, robotMap);
            }
            else {
                throw tcp::InvalidParametersException(std::to_string(parameters.size()));
            }
            break;
        }
    }
    catch (const std::invalid_argument &e) {
        std::string message = "Invalid argument";
        connection->send(message + e.what());
    }
    catch (const IdAlreadyDefinedException &e) {
        std::string message = "Tried to add robot, but the id was already in the map. Id: : ";
        connection->send(message + e.what());
    }
}

void parseMessage(tcp::Connection* connection, std::map<const int, int> &robot_map)
{
    std::vector<std::string> result;
    try {
        auto messages = connection->receive(MSG_DONTWAIT);
        for (const std::string &message : messages) {
            std::cout << message << "\n" << std::endl;
            result = split(message, ',');
            Function function = parse_function(result[0]);
            callFunction(function, result, robot_map, client_fd);
        }
    }
    catch (tcp::MalformedMessageException &e) {
        connection->send(e.what());
    }
    catch (tcp::UnreadableFunctionException &e) {
        connection->send(e.what());
    }
    catch (tcp::InvalidParametersException &e) {
        connection->send(e.what());
    }
    
    // TODO: Fix delete problem
    delete connection;
}

void tcp::PortService::start()
{
    std::cout << "Waiting for connections on port: " << server.get_port() << std::endl;
    while (true) {
        try {
            tcp::Connection* connection = server.accept();
            std::thread thread{parseMessage, connection, std::ref(robotMap)};
            thread.detach();
        }
        catch (tcp::AcceptException &e) {
            std::cerr << e.what() << "\n";
        }
    }
}

int main(int argc, char **argv)
{
    PortService portService(4444);
    portService.start();
}
