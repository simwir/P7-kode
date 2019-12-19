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
#ifndef ROBOT_OPTIONS_HPP
#define ROBOT_OPTIONS_HPP

#include <filesystem>
#include <getopt.h>
#include <string>

namespace robot {

struct Options {
    std::string time_addr = "127.0.0.1";
    std::string com_addr = "127.0.0.1";
    std::string pds_addr = "127.0.0.1";
    std::string order_addr = "127.0.0.1";
    std::string robot_addr = "127.0.0.1";

    std::string time_port = "5555";
    std::string com_port = "5435";
    std::string pds_port = "4444";
    std::string order_port = "7777";

    std::filesystem::path station_query_template =
        std::filesystem::path("station_scheduling.q.template");
    std::filesystem::path waypoint_query_template =
        std::filesystem::path("waypoint_scheduling.q.template");
    std::filesystem::path eta_query_template = std::filesystem::path("eta_query.q.template");
};

const char *const shortOpts = "t:sc:p:o:hr:qa:";
const option longOpts[] = {{"time-service", required_argument, nullptr, 't'},
                           {"system-time", no_argument, nullptr, 's'},
                           {"com-module", required_argument, nullptr, 'c'},
                           {"port-service", required_argument, nullptr, 'p'},
                           {"order-service", required_argument, nullptr, 'o'},
                           {"help", no_argument, nullptr, 'h'},
                           {"robot", required_argument, nullptr, 'r'},
                           {"station-query-file", required_argument, nullptr, 'q'},
                           {"waypoint-query-file", required_argument, nullptr, 'w'},
                           {"single-ip", required_argument, nullptr, 'a'}};
} // namespace robot
#endif
