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
#include "robot/options.hpp"
#include "robot/orchestrator.hpp"

#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <optional>
#include <utility>

using namespace robot;
Options options;

bool time_chosen = false;

void print_help(const char *const execute_location)
{
    std::cerr
        << "Usage: " << execute_location << " [options] <robot id> <webots world>(.wbt) [options]\n"
        << "-t --time-service <IP>[:<PORT>]  Set address of time service. Cannot be used with -s.\n"
        << "-s --system-time                 Use system time as time service. Cannot be used with "
           "-t. Not implemented.\n"
        << "-c --com-module <IP>[:<PORT>]    Set address of the communication modulde.\n"
        << "-p --port-service <IP>[:<PORT>]  Set address of the port discovery service.\n"
        << "-o --order-service <IP>[:<PORT>] Set address of the order service.\n"
        << "-a --single-ip <IP>              Set address of all modules and services.\n"
        << "-r --robot <IP>                  Set the host of the robot.\n"
        << "-q --station-query-file PATH     Path to the station query file template.\n"
        << "-w --waypoint-query-file PATH    Path to the waypoint query file template.\n"
        << "-e --eta-query-file PATH         Path to the eta query file template.\n"
        << "-h --help                        Print this help message" << std::endl;
}

std::pair<std::string, std::optional<std::string>> parse_address(std::string address)
{
    auto colon = address.find(":");
    if (colon == std::string::npos) {
        return std::pair(address, std::nullopt);
    }
    else {
        return std::pair(address.substr(0, colon), std::optional(address.substr(colon + 1)));
    }
}

int main(int argc, char **argv)
{
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
            options.time_addr = address.first;
            if (address.second) {
                options.time_port = address.second.value();
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
            options.com_addr = address.first;
            if (address.second) {
                options.com_port = address.second.value();
            }
            break;
        case 'p':
            address = parse_address(std::string{optarg});
            options.pds_addr = address.first;
            if (address.second) {
                options.pds_port = address.second.value();
            }
            break;
        case 'o':
            address = parse_address(std::string{optarg});
            options.order_addr = address.first;
            if (address.second) {
                options.order_port = address.second.value();
            }
            break;
        case 'a':
            if (time_chosen) {
                std::cerr << "Time system already set to system time, time system will not be set "
                             "to remote server."
                          << std::endl;
            }
            else {
                options.time_addr = std::string{optarg};
            }
            time_chosen = true;
            options.com_addr = options.pds_addr = options.order_addr = options.robot_addr =
                std::string{optarg};
        case 'r':
            options.robot_addr = std::string{optarg};
            break;
        case 'q':
            options.station_query_template = std::filesystem::path{optarg};
            break;
        case 'w':
            options.waypoint_query_template = std::filesystem::path{optarg};
            break;
        case 'e':
            options.eta_query_template = std::filesystem::path{optarg};
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

    int robot_id = std::stoi(argv[optind]);

    std::filesystem::path world_path{argv[optind + 1]};
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

    robot::Orchestrator orchestrator{robot_id, world_file, options};
    std::cerr << "starting orchestrator\n";
    orchestrator.main_loop();
}
