#ifndef TRANSLATOR_PORTSERVICE_HPP
#define TRANSLATOR_PORTSERVICE_HPP


#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include "server.hpp"

enum class Functions{ addRobot, getRobot, removeRobot};

class UnreadableFunctionException : public std::exception {
    std::string message;

public:
    UnreadableFunctionException(const std::string& in_message) { message = in_message; }
    const char* what() const noexcept override { return message.c_str(); }
};

class IdAlreadyDefinedException : public std::exception {
    std::string message;

public:
    IdAlreadyDefinedException(const std::string& in_message) { message = in_message; }
    const char* what() const noexcept override { return message.c_str(); }
};

class UnreadableParametersException : public std::exception {
    std::string message;

public:
    UnreadableParametersException(const std::string& in_message) { message = in_message; }
    const char* what() const noexcept override { return message.c_str(); }
};

class PortService {
public:
    PortService(int port);
    void start_server();
    ~PortService();
private:
    tcp::Server server;
    std::map<int, int> robotMap; // {Robot_id , Port_number}
};

Functions parseFunction(const std::string& function);
void callFunction(Functions function, const std::vector<std::string>& parameters);
int addRobot(int id);
bool registerRobot(int id, int port);

#endif //TRANSLATOR_PORTSERVICE_HPP
