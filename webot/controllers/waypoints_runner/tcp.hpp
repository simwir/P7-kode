#ifndef TCP_HPP
#define TCP_HPP

#include <optional>
#include <string>

#include "server.hpp"

enum class MessageType {get_position, set_destination, not_understood};

struct Message {
    std::string payload;
    MessageType type;
};

class Server {
public:
    Server(std::string id);
    ~Server();
    std::optional<Message> get_message();
    void send_message(Message);
private:
    tcp::Server server;
    std::string robot_id;
    int client_fd;
};

#endif
