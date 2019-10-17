#include "port_discovery_service.hpp"
#include "tcp_server.hpp"

#include <iostream>
#include <map>
#include <vector>
#include <iterator>

std::map<int, int> robotMap;

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

Functions parseFuncton(std::string function){
    if (function.compare("addRobot")){
        return Functions::addRobot;
    } else if (function.compare("getRobot")){
        return Functions::getRobot;
    } else if (function.compare("deleteRobot")){
        return Functions::removeRobot;
    } else{
        std::cout << "Invalid function";
    }
}

void parseMessage(std::string message){
    std::vector<std::string> result;

    result = split(message, ',');

    Functions function = parseFuncton(result[0]);
    callFunction(function, result);

}

bool addRobot(int id, int port){
    robotMap[id] = port;
    std::cout << robotMap[id] << '\n';
    return true;
}

int getRobot(int id) {
    return robotMap.at(id);
}

void removeRobot(int id){
    robotMap.erase(id);
}

void callFunction(Functions function, std::vector<std::string> parameters){
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

}

int main(int argc, char** argv){
    std::vector<std::string> result;

    int portNumber = 4444;
    int backlog = 2;
    std::string testMessage = "addRobot,2,4432";
    //TCPServer server{portNumber, backlog};
    parseMessage(testMessage);
    result = split(testMessage, ',');
    std::copy(result.begin(), result.end(),
              std::ostream_iterator<std::string>(std::cout, "\n"));
}