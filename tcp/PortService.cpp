#include "PortService.hpp"
#include "server.hpp"
#include "send.hpp"
#include "receive.hpp"

#include <iostream>
#include <map>
#include <vector>
#include <assert.h>
#include <unistd.h>
#include <thread>
#include <pthread.h>


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

Functions parseFunction(const std::string& function){
    if (function == "addRobot"){
        return Functions::addRobot;
    } else if (function == "getRobot"){
        return Functions::getRobot;
    } else if (function == "deleteRobot"){
        return Functions::removeRobot;
    } else{
        throw UnreadableFunctionException(function);
    }
}

void addRobot(int id, int port, std::map<int,int> &robotMap) {
    auto search = robotMap.find(id);
    if (search == robotMap.end()) {
        robotMap.insert(std::make_pair(id, port));
    } else {
        throw IdAlreadyDefinedException(std::to_string(id));
    }
}

int getRobot(int id, std::map<int,int> &robotMap){
    try {
        return robotMap.at(id);
    } catch (std::out_of_range& e){
        std::cerr << "No robot with id: " << e.what() << std::endl;
    }
}

void removeRobot(int id, std::map<int,int> &robotMap){
    robotMap.erase(id);
}

void callFunction(Functions function, const std::vector<std::string>& parameters, std::map<int,int> &robotMap){
    try {
        switch (function) {
            case Functions::getRobot:
                assert(parameters.size()== 2);
                getRobot(stoi(parameters[1]), robotMap);
                break;
            case Functions::addRobot:
                assert(parameters.size()== 3);
                addRobot(stoi(parameters[1]),stoi(parameters[2]), robotMap);
                break;
            case Functions::removeRobot:
                assert(parameters.size()== 2);
                removeRobot(stoi(parameters[1]), robotMap);
                break;
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << '\n';
    } catch (const IdAlreadyDefinedException& e) {
        std::cerr << "Tried to add robot, but the id was already in the map. Id: : " << e.what() << '\n';
    }
}

void parseMessage(int fd, std::map<int, int> &robotMap){
    auto message = tcp::receive(fd);
    auto result = split(message, ',');

    try {
        Functions function = parseFunction(result[0]);
        callFunction(function, result, robotMap);
    } catch (UnreadableFunctionException& e) {
        std::cout << "Unable to parse function" << std::endl;
        std::cout << e.what() << std::endl;
    } catch (UnreadableParametersException& e) {
        std::cout << "Unable to parse function" << std::endl;
    }

}

PortService::PortService(int port) : server(tcp::Server(port)) {}

PortService::~PortService() {
    server.close();
}

void PortService::start_server(){
    std::vector<std::thread> threads;
    std::cout << "Waiting for connections ..." << std::endl;

    while(true){
        try {
            int client_fd = server.accept();
            std::thread t1(parseMessage, client_fd, std::ref(robotMap)); //TODO: Remember mutex around robotMap
            t1.detach();
        } catch (tcp::AccpetException& e) {
            std::cerr << e.what() << "\n";
        }
    }
}


int main(int argc, char** argv){
    PortService portService(4444);
    portService.start_server();
}