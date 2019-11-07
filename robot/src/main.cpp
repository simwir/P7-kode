#include "robot/master.hpp"
#include "robot/config.hpp"

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "please give a path\n";
        exit(1);
    }
    std::filesystem::path world_file{argv[1]};
    std::filesystem::path out_file = "static_conf.json";

    robot::Master master{"localhost", "localhost", 0, std::cin};
    master.load_webots_to_config();
    master.write_static_config(out_file);
}
