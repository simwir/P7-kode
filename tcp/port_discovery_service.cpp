#include "port_discovery_service.hpp"
#include "tcp_server.hpp"

#include <iostream>
#include <map>
#include <vector>
#include <exception>
#include <assert.h>
#include <unistd.h>
#include <thread>


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

void addRobot(int id, int port){
    //robotMap.insert(std::make_pair(id,port));
}

int getRobot(int id) {
    try {
        return 1;//robotMap.at(id);
    } catch (std::out_of_range& e){
        std::cout << "No robot with requested id" << std::endl;
    }
}

void removeRobot(int id){
    //robotMap.erase(id);
}

void callFunction(Functions function, const std::vector<std::string>& parameters){
    try {
        switch (function) {
            case Functions::getRobot:
                assert(parameters.size()== 2);
                getRobot(stoi(parameters[1]));
                break;
            case Functions::addRobot:
                assert(parameters.size()== 3);
                addRobot(stoi(parameters[1]),stoi(parameters[2]));
                break;
            case Functions::removeRobot:
                assert(parameters.size()== 2);
                removeRobot(stoi(parameters[1]));
                break;
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << '\n';
    }
}

void parseMessage(int fd){


    auto result = split(message, ',');

    try {
        Functions function = parseFunction(result[0]);
        callFunction(function, result);
    } catch (UnreadableFunctionException& e) {
        std::cout << "Unable to parse function" << std::endl;
        std::cout << e.what() << std::endl;
    } catch (UnreadableParametersException& e) {
        std::cout << "Unable to parse function" << std::endl;
    }

    server.close_client(fd);
}


int main(int argc, char** argv){
    std::map<int, int> robotMap;
    const int portNumber = 4444;
    int client_fd;
    std::vector<std::thread> threads;

    TCPServer server{portNumber};


    puts("Waiting for connections ...");
    while(true){

        try {
            client_fd = server.accept();
            std::thread t1(parseMessage, client_fd)
            server.close_client(client_fd);
        } catch (TCPServerAcceptException& e) {
            std::cerr << e.what() << "\n";
        }
    }
}