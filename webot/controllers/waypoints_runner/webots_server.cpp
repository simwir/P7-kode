#include <sys/socket.h>

#include <iostream>

#include "webots_server.hpp"
#include "client.hpp"

const std::string PDS_PORT = "4444";
const std::string PDS_ADDR = "127.0.0.1";

using namespace webots_server;

webots_server::Server::Server(std::string id) : server(tcp::Server{0}), robot_id(id){
    tcp::Client client{PDS_ADDR, PDS_PORT};
    client.send("addRobot," + id + "," + std::to_string(server.get_port()));
    client.close();
    client_fd = server.accept();
}

webots_server::Server::~Server(){
    server.close();
    // Deregister robot with the port discovery service.
    tcp::Client client{PDS_ADDR, PDS_PORT};
    client.send("removeRobot," + robot_id);
    client.close();
}

std::vector<Message> webots_server::Server::get_messages(){
    std::vector<std::string> raw_messages = server.receive(client_fd, MSG_DONTWAIT); 
    std::vector<Message> messages;
    std::cerr << "Getting messages" << std::endl;
    for (std::string raw_message : raw_messages){
        MessageType messageType;
        size_t split_pos = raw_message.find(",");
        if (split_pos == std::string::npos){
            send_message(Message{raw_message, MessageType::not_understood});
            continue;
        }
        std::cerr << "get_message substr 1. pos: " << split_pos << " len: " << raw_message.length();
        std::string type = raw_message.substr(0, split_pos);
        if (type == "get_position") {
            messageType = MessageType::get_position;
        } else if (type == "set_destination") {
            messageType = MessageType::set_destination;
        } else {
            send_message(Message{raw_message, MessageType::not_understood});
            continue;
        }
        std::cerr << "get_message substr 2.";
        messages.push_back(Message{raw_message.substr(split_pos + 1),
                                   messageType});
    }
    return messages;
}

void webots_server::Server::send_message(Message message){
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

