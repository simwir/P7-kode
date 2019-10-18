#include "tcp.hpp"
#include "tcp/tcp_client.hpp"

constexpr std::string PDS_PORT = "4444";
constexpr std::string PDS_ADDR = "127.0.0.1";

tcp::tcp(std::string id){
    robot_id = id;
    server = TCPServer{0};
    TCPClient client{PDS_ADDR, PDS_PORT};
    client.send("addRobot," + id + "," + server.get_port());
    client.close();
    client_fd = server.accept();
}

tcp::~tcp(){
    server.close();
    // Deregister robot with the port discovery service.
    TCPClient client{PDS_ADDR, PDS_PORT};
    client.send("removeRobot," + robot_id);
    client.close();
}

std::optional<Message> tcp::get_message(){
    std::string raw_payload = server.receive(client_fd, ); //TODO: No block on receive.

    if (/*message received*/ true){
        MessageType messageType;
        size_t split_pos = raw_payload.fint(",");
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

void tcp::send_message(Message message){
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

