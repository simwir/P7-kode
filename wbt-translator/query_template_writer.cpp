
#include "query_template_writer.hpp"

#include <iostream>
#include <regex>
#include <sstream>
#include <string>

const std::regex template_par{TEMPL_START + R"_(.*)_" + TEMPL_END};
const std::regex strat_var_placeholder(TEMPL_START + R"(STRATEGY_VAR_LIST)" + TEMPL_END);

std::string get_var_list(Parser &parser);
std::ostream &other_robot_vars(std::ostream &os, size_t num_robots);
std::ostream &robot_vars(std::ostream &os);
std::ostream &visited_vars(std::ostream &os, size_t num_waypoints);
std::ostream &point_variables(std::ostream &os, size_t num_waypoints);

bool instantiate_query_template(Parser &parser, std::istream &is, std::ostream &os)
{
    std::string line;
    bool skipws = is.flags() & std::ios_base::skipws;
    is.unsetf(std::ios_base::skipws);
    while (std::getline(is, line)) {
        os << std::regex_replace(line, strat_var_placeholder, get_var_list(parser)) << '\n';
    }
    // false if loop stopped for other reason than end-of-file.
    if (skipws)
        is.setf(std::ios_base::skipws);
    return is.eof();
}

std::string get_var_list(Parser &parser)
{
    AST ast = parser.parse_stream();
    const size_t num_waypoints = ast.nodes.size();
    const size_t num_robots = ;//parser.number_of_robots;
    std::stringstream ss;
    ss << "{\\\n";
    other_robot_vars(ss, num_robots);
    robot_vars(ss);
    visited_vars(ss, num_waypoints);
    ss << "} -> {\\\n";
    point_variables(ss, num_waypoints);
    ss << "} ";

    return ss.str();
}

std::ostream &other_robot_vars(std::ostream &os, size_t num_robots)
{
    for (size_t i = 2; i <= num_robots; ++i) {
        os << "OtherRobot(" << i << ").location,\\\n"
           << "OtherRobot(" << i << ").cur,\\\n"
           << "OtherRobot(" << i << ").next.type,\\\n"
           << "OtherRobot(" << i << ").next.value,\\\n";
    }
    return os;
}

std::ostream &robot_vars(std::ostream &os)
{
    return os << "Robot.location,\\\nRobot.dest,\\\nRobot.cur_waypoint,\\\nRobot.dest_waypoint";
}

std::ostream &visited_vars(std::ostream &os, size_t num_waypoints)
{
    for (size_t i = 1; i <= num_waypoints; ++i) {
        os << "Robot.visited[" << i << "]\\\n";
    }
    return os;
}

std::ostream &point_variables(std::ostream &os, size_t num_waypoints)
{
    for (size_t i = 1; i <= num_waypoints; ++i) {
        os << "Waypoint(" << i << ").num_in_queue\\\n";
    }
    return os << "Robot.x\\\n";
}
