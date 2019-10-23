#include <sys/socket.h>

#include "tcp.hpp"
#include "client.hpp"

const std::string PDS_PORT = "4444";
const std::string PDS_ADDR = "127.0.0.1";

Server::Server(std::string id) : server(tcp::Server{0}){
    robot_id = id;
    tcp::Client client{PDS_ADDR, PDS_PORT};
    client.send("addRobot," + id + "," + std::to_string(server.get_port()));
    client.close();
    client_fd = server.accept();
}

Server::~Server(){
    server.close();
    // Deregister robot with the port discovery service.
    tcp::Client client{PDS_ADDR, PDS_PORT};
    client.send("removeRobot," + robot_id);
    client.close();
}

std::optional<Message> Server::get_message(){
    std::string raw_payload = server.receive(client_fd, MSG_DONTWAIT); //TODO: No block on receive.

    if (/*message received*/ true){
        MessageType messageType;
        size_t split_pos = raw_payload.find(",");
        if (split_pos == std::string::npos){
            send_message(Message{raw_payload, MessageType::not_understood});
            return std::nullopt;
        }
        std::string type = raw_payload.substr(0, split_pos);
        if (type == "get_position") {
            messageType = MessageType::get_position;
        } else if (type == "set_destination") {
            messageType = MessageType::set_destination;
        } else {
            send_message(Message{raw_payload, MessageType::not_understood});
            return std::nullopt;
        }
        return Message{raw_payload.substr(split_pos + 1, raw_payload.length()),
                       messageType};
    } else {
        return std::nullopt;
    }
}

void Server::send_message(Message message){
    std::string payload;
    switch (message.type) {
    case MessageType::get_position:
        payload = "get_position," + message.payload;
        break;
    case MessageType::set_destination:
        payload = "set_destination," + message.payload;
        break;
    case MessageType::not_understood:
        payload = "not_understood," + message.payload;
    }
    server.send(client_fd, payload);
}

