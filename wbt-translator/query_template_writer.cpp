
#include "query_template_writer.hpp"

#include <iostream>
#include <regex>
#include <sstream>
#include <string>

const std::regex template_par{TEMPL_START + R"_(.*)_" + TEMPL_END};
const std::regex strat_var_placeholder(TEMPL_START + R"(STRATEGY_VAR_LIST)" + TEMPL_END);
// Extra backslash so verifyta doesn't get confused by newline.
constexpr auto varsep = ",";
constexpr auto newline = "\\\n";

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
    const size_t num_robots = 8; // parser.number_of_robots;
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
        os << "OtherRobot(" << i << ").location" << varsep << newline
           << "OtherRobot(" << i << ").cur" << varsep << newline
           << "OtherRobot(" << i << ").next.type" << varsep << newline
           << "OtherRobot(" << i << ").next.value" << varsep << newline;
    }
    return os;
}

std::ostream &robot_vars(std::ostream &os)
{
    return os << "Robot.location" << varsep << newline
              << "Robot.dest" << varsep << newline
              << "Robot.cur_waypoint" << varsep << newline
              << "Robot.dest_waypoint" << varsep << newline;
}

std::ostream &visited_vars(std::ostream &os, size_t num_waypoints)
{
    for (size_t i = 1; i <= num_waypoints; ++i) {
        os << "Robot.visited[" << i << "]";
        if (i != num_waypoints) os << varsep;
        os << newline;
    }
    return os;
}

std::ostream &point_variables(std::ostream &os, size_t num_waypoints)
{
    for (size_t i = 1; i <= num_waypoints; ++i) {
        os << "Waypoint(" << i << ").num_in_queue" << varsep << newline;
    }
    return os << "Robot.x" << newline;
}
