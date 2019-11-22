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
#include <getopt.h>
#include <iostream>
#include <utility>
#include <optional>

std::string time_addr = "127.0.0.1";
std::string com_addr = "127.0.0.1";
std::string pds_addr = "127.0.0.1";
std::string order_addr = "127.0.0.1";
std::string robot_addr = "127.0.0.1";

std::string time_port = "5555";
std::string com_port = "5435";
std::string pds_port = "4444";
std::string order_port = "7777";

bool time_chosen = false;

void print_help(const char *const execute_location)
{
    std::cerr
        << "Usage: " << execute_location << " [options] <webots world>(.wbt) [options]\n"
        << "-t --time-service <IP>[:<PORT>]  Set address of time service. Cannot be used with -s.\n"
        << "-s --system-time                 Use system time as time service. Cannot be used with "
           "-t. Not implemented.\n"
        << "-c --com-module <IP>[:<PORT>]    Set address of the communication modulde.\n"
        << "-p --port-service <IP>[:<PORT>]  Set address of the port discovery service.\n"
        << "-o --order-service <IP>[:<PORT>] Set address of the order service.\n"
        << "-r --robot <IP>                  Set the host of the robot.\n"
        << "-h --help                        Print this help message"
        << std::endl;
}

std::pair<std::string, std::optional<std::string>> parse_address(std::string address) {
    auto colon = address.find(":");
    if (colon == std::string::npos){
        return std::pair(address, std::nullopt);
    } else {
        return std::pair(address.substr(0, colon), std::optional(address.substr(colon + 1)));
    }
}

int main(int argc, char **argv)
{
    const char *const shortOpts = "t:sc:p:o:hr";
    const option longOpts[] = {{"time-service", required_argument, nullptr, 't'},
                               {"system-time", no_argument, nullptr, 's'},
                               {"com-module", required_argument, nullptr, 'c'},
                               {"port-service", required_argument, nullptr, 'p'},
                               {"order-service", required_argument, nullptr, 'o'},
                               {"help", no_argument, nullptr, 'h'},
                               {"robot", required_argument, nullptr, 'r'}};

    std::pair<std::string, std::optional<std::string>> address;
    int opt;
    while ((opt = getopt_long(argc, argv, shortOpts, longOpts, nullptr)) != -1) {
        switch (opt) {
        case 't':
            if (time_chosen) {
                std::cerr << "Time system already set to system time." << std::endl;
                exit(1);
            }
            time_chosen = true;
            address = parse_address(std::string{optarg});
            time_addr = address.first;
            if (address.second) {
                time_port = address.second.value();
            }
            break;
        case 's':
            if (time_chosen) {
                std::cerr << "Time system already set to time address." << std::endl;
                exit(1);
            }
            time_chosen = true;
            std::cerr << "System time service not implemented." << std::endl;
            exit(1);
            break;
        case 'c':
            address = parse_address(std::string{optarg});
            com_addr = address.first;
            if (address.second) {
                com_port = address.second.value();
            }
            break;
        case 'p':
            address = parse_address(std::string{optarg});
            pds_addr = address.first;
            if (address.second) {
                pds_port = address.second.value();
            }
            break;
        case 'o':
            address = parse_address(std::string{optarg});
            order_addr = address.first;
            if (address.second) {
                order_port = address.second.value();
            }
            break;
        case 'r':
            robot_addr = std::string{optarg};
            break;
        case 'h':
            print_help(argv[0]);
            exit(0);
            break;
        }
    }

    if (optind == argc) {
        print_help(argv[0]);
        exit(1);
    }

    std::filesystem::path world_path{argv[optind]};
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

    robot::Orchestrator orchestrator{robot_addr, com_addr, 1, world_file, time_addr};
    std::cerr << "starting orchestrator\n";
    orchestrator.main_loop();
}
