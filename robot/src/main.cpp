#include "robot/config.hpp"
#include "robot/orchestrator.hpp"

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "please give a path\n";
        exit(1);
    }
    std::filesystem::path world_file{argv[1]};
    std::filesystem::path out_file = "static_conf.json";

    robot::Orchestrator orchestrator{"localhost", "localhost", 0, std::cin};
    orchestrator.load_webots_to_config(world_file);
    orchestrator.write_static_config(out_file);
}
