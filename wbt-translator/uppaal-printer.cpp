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
