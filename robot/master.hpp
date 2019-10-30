#ifndef MASTER_HPP
#define MASTER_HPP

#include "config.hpp"
#include "../tcp/include/client.hpp"

namespace robot {
class RecievedMessageException : public std::exception {
    std::string message;
    
    public:
        RecievedMessageException(const std::string &in_message) : message(in_message) {}

        const char *what() const noexcept
        {
            return message.c_str();
        }
};

class Master{
    public:
        Master(std::string robot_host, std::string broadcast_host, int robot_id);
        void load_webots_to_config(std::string input_file, std::string output_file);
        void request_broadcast_info();
        std::string recv_broadcast();

    private:
        robot::Config config;
        tcp::Client webot_client;
        tcp::Client broadcast_client;
        
};
}// namespace robot
#endif