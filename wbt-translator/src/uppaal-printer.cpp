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
#include "wbt-translator/uppaal-printer.hpp"

#include <sstream>

std::string print_waypoints_of_type(const AST &ast, const WaypointType type)
{
    std::stringstream ss1, ss2;
    int count = 0;
    for (auto &[id, waypoint] : ast.nodes) {
        if (waypoint.waypointType == type) {
            if (count != 0)
                ss2 << ",";
            count++;
            ss2 << id;
        }
    }
    std::string type_name;
    switch (type) {
    case WaypointType::eVia:
        type_name = "VIAS";
        break;
    case WaypointType::eStation:
        type_name = "STATIONS";
        break;
    case WaypointType::eEndPoint:
        type_name = "ENDPOINTS";
        break;
    }
    ss1 << "const int NUM_" << type_name << " = " << count << ";\n"
        << "const int " << type_name << "[NUM_" << type_name << "] = {" << ss2.str() << "};\n";
    return ss1.str();
}
