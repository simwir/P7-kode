#include <sys/socket.h>

#include <iostream>

#include "client.hpp"
#include "webots_server.hpp"

const std::string PDS_PORT = "4444";
const std::string PDS_ADDR = "127.0.0.1";

using namespace webots_server;

webots_server::Server::Server(std::string id)
    : server(tcp::Server{0}), robot_id(id) {
  tcp::Client port_discovery{PDS_ADDR, PDS_PORT};
  port_discovery.send("add_robot," + id + "," +
                      std::to_string(server.get_port()));
  client = server.accept();
}

webots_server::Server::~Server() {
  server.close();
  // Deregister robot with the port discovery service.
  tcp::Client client{PDS_ADDR, PDS_PORT};
  client.send("remove_robot," + robot_id);
  client.close();
}

std::vector<Message> webots_server::Server::get_messages() {
  std::vector<std::string> raw_messages = client->receive();
  std::vector<Message> messages;
  for (const auto &raw_message : raw_messages) {
    MessageType message_type;
    size_t split_pos = raw_message.find(",");
    // If no ',' found we assume this to be a command without an argument.
    if (split_pos == std::string::npos) {
      if (raw_message == "get_position") {
        message_type = MessageType::get_position;
      } else {
        send_message(Message{raw_message, MessageType::not_understood});
        std::cerr << "Recieved message not understood: " << raw_message << std::endl;
        continue;
      }
    } else {
      std::string type = raw_message.substr(0, split_pos);
      if (type == "set_destination") {
        message_type = MessageType::set_destination;
      } else {
        send_message(Message{raw_message, MessageType::not_understood});
        std::cerr << "Recieved message not understood: " << raw_message << std::endl;
        continue;
      }
    }
    messages.push_back(
        Message{raw_message.substr(split_pos + 1), message_type});
  }
  return messages;
}

void webots_server::Server::send_message(const Message& message) {
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
  client->send(payload);
}
