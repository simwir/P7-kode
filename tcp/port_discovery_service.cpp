#include "port_discovery_service.hpp"
#include "tcp_server.hpp"

#include <iostream>
#include <map>
#include <vector>
#include <iterator>
#include <exception>

std::map<int, int> robotMap;


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
    if (function == ("addRobot")){
        return Functions::addRobot;
    } else if (function == ("getRobot")){
        return Functions::getRobot;
    } else if (function == ("deleteRobot")){
        return Functions::removeRobot;
    } else{
        throw UnreadableFunctionException(function);
    }
}

void parseMessage(const std::string& message){
    std::vector<std::string> result;

    result = split(message, ',');

    try {
        Functions function = parseFunction(result[0]);
        callFunction(function, result);
    } catch (UnreadableFunctionException& e) {
        std::cout << "Unable to parse function" << std::endl;
        std::cout << e.what() << std::endl;
    } catch (UnreadableParametersException& e) {
        std::cout << "Unable to parse function" << std::endl;
    }

}

void addRobot(int id, int port){
    robotMap.insert(std::pair<int,int>(id,port));
}

int getRobot(int id) {
    try {
        return robotMap.at(id);
    } catch (std::out_of_range& e){
        std::cout << "No robot with requested id" << std::endl;
    }
}

void removeRobot(int id){
    robotMap.erase(id);
}

void callFunction(Functions function, const std::vector<std::string>& parameters){
    try {
        switch (function) {
            case Functions::getRobot:
                getRobot(stoi(parameters[1]));
                break;
            case Functions::addRobot:
                addRobot(stoi(parameters[1]),stoi(parameters[2]));
            case Functions::removeRobot:
                removeRobot(stoi(parameters[1]));
                break;
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << '\n';
    }
}

int main(int argc, char** argv){
    std::vector<std::string> result;
    const int portNumber = 4444;

    std::string testMessage = "addRobot,2,4432";

    TCPServer server{portNumber};

    int client_fd = 0;

    while(true) {
        std::string message;
        client_fd = server.accept();

        message = server.receive(client_fd);

        parseMessage(message);

        server.close();
    }
}