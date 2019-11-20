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
#include "config/config.hpp"
#include "robot/orchestrator.hpp"

#include <filesystem>
#include <fstream>

int main(int argc, char **argv)
{
    if (argc < 3 || argc > 4) {
        std::cerr << "Usage:\n"
                  << "\t" << argv[0] << " <webots world (.wbt)> <IP> [<webots closk host>]";
        exit(1);
    }
    std::filesystem::path world_path{argv[1]};
    if (!std::filesystem::exists(world_path)) {
        std::cerr << "Cannot find file " << world_path << std::endl;
        exit(1);
    }
    if (world_path.extension() != ".wbt") {
        std::cerr << "Expected webots world file (*.wbt)" << std::endl;
        exit(1);
    }

    std::cerr << "constructing orchestrator... ";
    std::ifstream world_file{world_path};

    std::string clock_host = argc == 4 ? argv[3] : "127.0.0.1";

    robot::Orchestrator orchestrator{argv[2], argv[2], 1, world_file};
    std::cerr << "starting orchestrator\n";
    orchestrator.main_loop();
}
