#ifndef TRANSLATOR_PORT_DISCOVERY_SERVICE_HPP
#define TRANSLATOR_PORT_DISCOVERY_SERVICE_HPP


#include <stdio.h>
#include <string>
#include <map>
#include <vector>

enum class Functions{ addRobot, getRobot, removeRobot};

class UnreadableFunctionException : public std::exception {
    std::string message;

public:
    UnreadableFunctionException(const std::string& in_message) { message = in_message; }
    const char* what() const noexcept override { return message.c_str(); }
};

class UnreadableParametersException : public std::exception {
    std::string message;

public:
    UnreadableParametersException(const std::string& in_message) { message = in_message; }
    const char* what() const noexcept override { return message.c_str(); }
};

class port_discovery_service {
public:
private:
    std::map<int, int> robotMap;
};

Functions parseFunction(const std::string& function);
void parseMessage(const std::string& message);
void callFunction(Functions function, const std::vector<std::string>& parameters);
int addRobot(int id);
bool registerRobot(int id, int port);

#endif //TRANSLATOR_PORT_DISCOVERY_SERVICE_HPP
