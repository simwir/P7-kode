#ifndef MASTER_HPP
#define MASTER_HPP

#include "config.hpp"
#include "../tcp/include/client.hpp"

namespace robot {
class Master{
    public:
        Master(int robot_id);
        void load_webots_to_config(std::string input_file, std::string output_file);
        

    private:
        robot::Config config;
        
};
}// namespace robot
#endif