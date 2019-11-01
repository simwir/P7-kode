#include "xml-generator.hpp"
#include "distance_matrix.hpp"
#include "uppaal-printer.hpp"

int get_count_of_type(const AST &ast, const WaypointType type){
    int count = 0;
    for (auto &[id, waypoint] : ast.nodes) {
        if (waypoint.waypointType == type) {
            if (count != 0)
            count++;
        }
    }
    return count;
}

void generate_xml(const AST& ast, int number_of_robots, std::ostream &os, std::istream &is){
    while(!is.eof()){
        std::string line;
        std::getline(is, line);
        const std::regex stations{"%¤NUM_STATIONS¤%"};
        const std::regex end_stations{"%¤NUM_END_STATIONS¤%"};
        const std::regex robots{"%¤NUM_ROBOTS¤%"};
        const std::regex waypoints{"%¤NUM_WAYPOINTS¤%"};
        const std::regex waypoint_dist{"%¤WAYPOINT_DIST¤%"};
        const std::regex station_waypoints{"%¤STATION_WAYPOINTS¤%"};
        line = std::regex_replace(line,
                                  stations,
                                  std::to_string(get_count_of_type(ast, WaypointType::eStation)));
        line = std::regex_replace(line,
                                  end_stations,
                                  std::to_string(get_count_of_type(ast, WaypointType::eEndPoint)));
        line = std::regex_replace(line,
                                  robots,
                                  std::to_string(number_of_robots));
        line = std::regex_replace(line,
                                  waypoints,
                                  std::to_string(get_count_of_type(ast, WaypointType::eVia) +
                                                 get_count_of_type(ast, WaypointType::eStation) +
                                                 get_count_of_type(ast, WaypointType::eEndPoint)));
        line = std::regex_replace(line,
                                  waypoint_dist,
                                  distance_matrix{ast}.to_uppaal_declaration());
        line = std::regex_replace(line,
                                  station_waypoints,
                                  print_waypoints_of_type(ast, WaypointType::eStation));
        os << line << "\n";
    }
}
