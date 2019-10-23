#include "port_service.hpp"
#include "server.hpp"
#include "send.hpp"
#include "receive.hpp"

#include <iostream>
#include <map>
#include <vector>
#include <assert.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <sys/socket.h>


//This function is duplicated from feature/tcp-class
std::vector<std::string> split(const std::string &input, char delimiter) {
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

Functions parse_function(const std::string& function){
    if (function == "add_robot"){
        return Functions::add_robot;
    } else if (function == "get_robot"){
        return Functions::get_robot;
    } else if (function == "remove_robot"){
        return Functions::remove_robot;
    } else{
        throw UnreadableFunctionException(function);
    }
}

void addRobot(int id, int port, std::map<const int,int> &robot_map) {
    std::mutex mutex;
    auto search = robot_map.find(id);
    if (search == robot_map.end()) {
        mutex.lock();
        std::cout << "Robot added with id: " << id << " and port: " << port << std::endl;
        robot_map.insert(std::make_pair(id, port));
        mutex.unlock();
    } else {
        throw IdAlreadyDefinedException(std::to_string(id));
    }
}

void get_robot(int id, std::map<const int,int> &robot_map, int fd){
    try {
        const std::string message = std::to_string(robot_map.at(id));
        tcp::send(fd, message);
    } catch (std::out_of_range& e){
        tcp::send(fd, "No robot with Id " + std::to_string(id));
    }
}

void remove_robot(int id, std::map<const int,int> &robot_map){
    std::mutex mutex;

    mutex.lock();
    robot_map.erase(id);
    mutex.unlock();
}

void callFunction(Functions function, const std::vector<std::string>& parameters, std::map<const int,int> &robotMap, int fd){
    try {
        switch (function) {
            case Functions::get_robot:
                assert(parameters.size()== 2);
                get_robot(stoi(parameters[1]), robotMap, fd);
                break;
            case Functions::add_robot:
                assert(parameters.size()== 3);
                addRobot(stoi(parameters[1]),stoi(parameters[2]), robotMap);
                break;
            case Functions::remove_robot:
                assert(parameters.size()== 2);
                remove_robot(stoi(parameters[1]), robotMap);
                break;
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << '\n';
    } catch (const IdAlreadyDefinedException& e) {
        std::cerr << "Tried to add robot, but the id was already in the map. Id: : " << e.what() << '\n';
    }
}

void parseMessage(int fd, std::map<const int, int> &robot_map){
    std::vector<std::string> result;
    try {
        auto messages = tcp::receive(fd, MSG_DONTWAIT);
        for (const std::string& message : messages){
            std::cout << message << "\n" << std::endl;
            result = split(message, ',');
            Functions function = parse_function(result[0]);
            callFunction(function, result, robot_map, fd);
        }
    } catch (tcp::MalformedMessageException& e) {
        std::cerr << e.what();
    } catch (UnreadableFunctionException& e) {
        std::cerr << "Unable to parse function" << e.what() << std::endl;
    } catch (UnreadableParametersException& e) {
        std::cout << "Unable to parse parameters" << std::endl;
    }
    close(fd);
}

port_service::port_service(int port) : server(tcp::Server(port)) {}

port_service::~port_service() {
    server.close();
}

void port_service::start_server(){
    std::cout << "Waiting for connections on port: " << server.get_port() << std::endl;
    while(true){
        try {
            int client_fd = server.accept();
            std::thread t1(parseMessage, client_fd, std::ref(robotMap));
            t1.detach();
        } catch (tcp::AcceptException& e) {
            std::cerr << e.what() << "\n";
        }
    }
}

int main(int argc, char** argv){
    port_service portService(4444);
    portService.start_server();
}