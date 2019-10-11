#include "uppaal-printer.hpp"

#include <sstream>

std::string print_station_of_type(const AST& ast, const WaypointType type) {
    std::stringstream ss1, ss2;
    int count = 0;
    for (auto it = ast.nodes.begin(); it != ast.nodes.end(); it++){
        if (it->second.waypointType == type) {
            if (count != 0)
                ss2 << ",";
            count++;
            ss2 << it->first;
        }
    }
    std::string type_name;
    switch (type){
    case eWaypoint:
        type_name = "WAYPOINTS";
        break;
    case eStation:
        type_name = "STATIONS";
        break;
    case eEndPoint:
        type_name = "ENDPOINTS";
        break;
    }
    ss1 << "const int NUM_" << type_name << " = " << count << ";\n"
        << "const int " << type_name << "[NUM_" << type_name << "] = {" << ss2.str() << "};\n";
    return ss1.str();
}
