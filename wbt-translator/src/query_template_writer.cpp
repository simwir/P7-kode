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

#include "wbt-translator/query_template_writer.hpp"

#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <util/file_lock.hpp>

const std::regex template_par{TEMPL_START + R"_(.*)_" + TEMPL_END};
const std::regex strat_var_placeholder(TEMPL_START + R"(STRATEGY_VAR_LIST)" + TEMPL_END);

constexpr auto varsep = ",";
// Extra backslash so verifyta doesn't get confused by newline.
constexpr auto newline = "\\\n";

std::string get_var_list(size_t num_robots, size_t num_waypoints);
std::ostream &other_robot_vars(std::ostream &os, size_t num_robots);
std::ostream &robot_vars(std::ostream &os);
std::ostream &visited_vars(std::ostream &os, size_t num_waypoints);
std::ostream &point_variables(std::ostream &os, size_t num_waypoints);

bool instantiate_query_template(size_t num_robots, size_t num_waypoints, std::istream &is,
                                std::ostream &os)
{
    std::string line;
    while (std::getline(is, line)) {
        os << std::regex_replace(line, strat_var_placeholder,
                                 get_var_list(num_robots, num_waypoints))
           << '\n';
    }
    // false if loop stopped for other reason than end-of-file.
    return is.eof();
}

bool instantiate_query_template(size_t num_robots, size_t num_waypoints,
                                const std::filesystem::path &template_path,
                                const std::filesystem::path &output_path)
{
    FileLock lock{output_path};
    std::ifstream in_file{template_path};
    std::ofstream out_file{output_path};
    return instantiate_query_template(num_robots, num_waypoints, in_file, out_file);
}

std::string get_var_list(size_t num_robots, size_t num_waypoints)
{
    std::stringstream ss;
    ss << "{" << newline;
    other_robot_vars(ss, num_robots);
    robot_vars(ss);
    visited_vars(ss, num_waypoints);
    ss << "} -> {" << newline;
    point_variables(ss, num_waypoints);
    ss << "} ";

    return ss.str();
}

std::ostream &other_robot_vars(std::ostream &os, size_t num_robots)
{
    for (size_t i = 2; i <= num_robots; ++i) {
        os << "OtherRobot(" << i << ").location" << varsep << newline << "OtherRobot(" << i
           << ").cur" << varsep << newline << "OtherRobot(" << i << ").next.type" << varsep
           << newline << "OtherRobot(" << i << ").next.value" << varsep << newline;
    }
    return os;
}

std::ostream &robot_vars(std::ostream &os)
{
    return os << "Robot.location" << varsep << newline << "Robot.dest" << varsep << newline
              << "Robot.cur_waypoint" << varsep << newline << "Robot.dest_waypoint" << varsep
              << newline;
}

std::ostream &visited_vars(std::ostream &os, size_t num_waypoints)
{
    for (size_t i = 0; i < num_waypoints; ++i) {
        os << "Robot.visited[" << i << "]";
        if (i != num_waypoints - 1)
            os << varsep;
        os << newline;
    }
    return os;
}

std::ostream &point_variables(std::ostream &os, size_t num_waypoints)
{
    for (size_t i = 0; i < num_waypoints; ++i) {
        os << "Waypoint(" << i << ").num_in_queue" << varsep << newline;
    }
    return os << "Robot.x" << newline;
}
