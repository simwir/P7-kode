/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler
 *Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 *associated documentation files (the "Software"), to deal in the Software without restriction,
 *including without limitation the rights to use, copy, modify, merge, publish, distribute,
 *sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or
 *substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 *NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
 *OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <sys/socket.h>

#include <iostream>

#include "tcp/client.hpp"
#include "webots_server.hpp"

const std::string PDS_PORT = "4444";
const std::string PDS_ADDR = "172.22.0.151";

using namespace webots_server;

webots_server::Server::Server(std::string id) : server(tcp::Server{0}), robot_id(id)
{
    tcp::Client port_discovery{PDS_ADDR, PDS_PORT};
    port_discovery.send("add_robot," + id + "," + std::to_string(server.get_port()));
    client = server.accept();
}

webots_server::Server::~Server()
{
    server.close();
    // Deregister robot with the port discovery service.
    tcp::Client client{PDS_ADDR, PDS_PORT};
    client.send("remove_robot," + robot_id);
    client.close();
}

std::optional<Message> webots_server::Server::get_message()
{
    auto msg = client->receive_nonblocking();
    if (!msg)
        return std::nullopt;
    std::string message = *msg;
    MessageType message_type;
    size_t split_pos = message.find(",");
    // If no ',' found we assume this to be a command without an argument.
    if (split_pos == std::string::npos) {
        if (message == "get_state") {
            message_type = MessageType::get_state;
        }
        else if (message == "done") {
            message_type = MessageType::done;
        }
        else {
            send_message(Message{message, MessageType::not_understood});
            std::cerr << "Recieved message not understood: " << message << std::endl;
            return std::nullopt;
        }
    }
    else {
        std::string type = message.substr(0, split_pos);
        std::cout << message << std::endl;
        if (type == "set_destination") {
            message_type = MessageType::set_destination;
        }
        else {
            send_message(Message{message, MessageType::not_understood});
            std::cerr << "Recieved message not understood: " << message << std::endl;
            return std::nullopt;
        }
    }
    return Message{message.substr(split_pos + 1), message_type};
}

void webots_server::Server::send_message(const Message &message)
{
    client->send(message.payload);
}
