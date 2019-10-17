#ifndef TRANSLATOR_PORT_DISCOVERY_SERVICE_HPP
#define TRANSLATOR_PORT_DISCOVERY_SERVICE_HPP


#include <stdio.h>
#include <string>
#include <map>
#include <vector>

enum class Functions{ addRobot, getRobot, removeRobot};

class port_discovery_service {
public:
    port_discovery_service(std::string host, std::string port);
    int addRobot(int id);
    bool registerRobot(int id, int port);

private:
    std::map<int, int> robotMap;
};

Functions parseFuncton(std::string function);
void parseMessage(std::string message);
void callFunction(Functions function, std::vector<std::string> parameters);

#endif //TRANSLATOR_PORT_DISCOVERY_SERVICE_HPP
