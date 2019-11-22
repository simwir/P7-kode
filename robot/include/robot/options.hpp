#ifndef ROBOT_OPTIONS_HPP
#define ROBOT_OPTIONS_HPP

#include <getopt.h>
#include <string>
#include <filesystem>

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
    std::filesystem::path eta_query_template =
        std::filesystem::path("eta_query.q.template");
};

const char *const shortOpts = "t:sc:p:o:hrq";
const option longOpts[] = {{"time-service", required_argument, nullptr, 't'},
                           {"system-time", no_argument, nullptr, 's'},
                           {"com-module", required_argument, nullptr, 'c'},
                           {"port-service", required_argument, nullptr, 'p'},
                           {"order-service", required_argument, nullptr, 'o'},
                           {"help", no_argument, nullptr, 'h'},
                           {"robot", required_argument, nullptr, 'r'},
                           {"station-query-file", required_argument, nullptr, 'q'},
                           {"waypoint-query-file", required_argument, nullptr, 'w'}};
} // namespace robot
#endif
