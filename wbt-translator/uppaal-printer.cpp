#include "uppaal-printer.hpp"

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
        type_name = "via";
        break;
    case WaypointType::eStation:
        type_name = "station";
        break;
    case WaypointType::eEndPoint:
        type_name = "endpoint";
        break;
    }
    ss1 << "const waypoint_t " << type_name << "_waypoint[" << type_name << "_t] = {" << ss2.str() << "};";
    return ss1.str();
}
