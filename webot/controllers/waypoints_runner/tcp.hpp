#ifndef TCP_HPP
#define TCP_HPP

#include <optional>
#include <string>

#include "tcp/tcp_server.hpp"

enum class MessageType {get_position, set_destination, not_understood};

struct Message {
    std::string payload;
    MessageType type;
};

class tcp {
public:
    tcp(std::string id);
    std::optional<Message> get_message();
    void send_message(Message);
private:
    TCPServer server;
    std::string robot_id;
    int client_fd;
};

#endif
