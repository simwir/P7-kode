#ifndef TCP_HPP
#define TCP_HPP

#include <vector>
#include <string>

#include "server.hpp"

namespace webots_server {

enum class MessageType {get_position, set_destination, not_understood};

struct Message {
    std::string payload;
    MessageType type;
};

class Server {
public:
    Server(std::string id);
    ~Server();
    std::vector<Message> get_messages();
    void send_message(Message);
private:
    tcp::Server server;
    std::string robot_id;
    int client_fd;
};
}
#endif
