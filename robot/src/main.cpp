#include "robot/config.hpp"
#include "robot/master.hpp"

#include <fstream>

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "please give a path\n";
        exit(1);
    }
    std::filesystem::path world_path{argv[1]};
    std::filesystem::path out_file = "static_conf.json";

    std::cerr << "constructing master... ";
    std::ifstream world_file{world_path};
    robot::Master master{"172.22.0.151", "172.22.0.151", 1, world_file};
    // master.load_webots_to_config();
    // master.write_static_config(out_file);
    std::cerr << "starting master\n";
    master.main_loop();
}
