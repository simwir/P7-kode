#include "robot/config.hpp"
#include "robot/master.hpp"

#include <fstream>

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "please give a path and an IP address in that order\n";
        exit(1);
    }
    std::filesystem::path world_path{argv[1]};
    std::filesystem::path out_file = "static_conf.json";

    std::ifstream world_file{world_path};
    robot::Master master{argv[2], argv[2], 1, world_file};
    // master.load_webots_to_config();
    // master.write_static_config(out_file);
    master.main_loop();
}
