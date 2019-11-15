
#include <fstream>

#include "robot/config.hpp"
#include "robot/info.hpp"
#include "robot/orchestrator.hpp"
#include "tcp/client.hpp"
#include "wbt-translator/apsp.hpp"
#include "wbt-translator/distance_matrix.hpp"
#include "wbt-translator/webots_parser.hpp"

#define PORT_TO_BROADCASTER "5435"
#define PORT_TO_PDS "4444"

robot::Orchestrator::Orchestrator(const std::string &robot_host, const std::string &broadcast_host,
                                  int robot_id, std::istream &world_file)
    : broadcast_client(broadcast_host, PORT_TO_BROADCASTER), webots_parser(world_file)
{
    // Connecting to the Port Discovery Service
    tcp::Client PDSClient{robot_host, PORT_TO_PDS};
    PDSClient.send("get_robot," + std::to_string(robot_id));
    std::string port_to_controller = PDSClient.receive_blocking();

    // Connecting to the WeBots Controller
    // webot_client = std::make_unique<tcp::Client>(robot_host, port_to_controller);
}

void robot::Orchestrator::add_waypoint_matrix(const AST &ast, int waypoint_count)
{
    // Get distance matrix for waypoints
    std::vector<std::vector<double>> waypoint_matrix = distance_matrix{ast}.get_data();

    // Convert waypoint distance matrix.
    Json::Value jsonarray_waypoint_matrix{Json::arrayValue};
    for (size_t i = 0; i < waypoint_count; i++) {
        Json::Value jsonarray_waypoint_row{Json::arrayValue};
        for (size_t j = 0; j < waypoint_count; j++) {
            jsonarray_waypoint_row.append(waypoint_matrix.at(i).at(j));
        }
        jsonarray_waypoint_matrix.append(jsonarray_waypoint_row);
    }
    static_config.set("waypoint_distance_matrix", jsonarray_waypoint_matrix);
}

void robot::Orchestrator::add_station_matrix(const AST &ast, int waypoint_count)
{
    // Get distance matrix for stations
    std::map<int, std::map<int, double>> apsp_distances = all_pairs_shortest_path(ast).dist;

    // Convert shortest paths between stations
    Json::Value jsonarray_apsp_distances{Json::arrayValue};
    for (size_t i = 0; i < waypoint_count; i++) {
        if (ast.nodes.at(i).waypointType == WaypointType::eStation) {
            Json::Value jsonarray_apsp_row{Json::arrayValue};
            for (size_t j = 0; j < waypoint_count; j++) {
                if (ast.nodes.at(j).waypointType == WaypointType::eStation) {
                    jsonarray_apsp_row.append(apsp_distances.at(i).at(j));
                }
            }
            jsonarray_apsp_distances.append(jsonarray_apsp_row);
        }
    }
    static_config.set("station_distance_matrix", jsonarray_apsp_distances);
}

void robot::Orchestrator::dump_waypoint_info(const AST &ast)
{
    // Dump all waypoint information.
    Json::Value waypoint_list{Json::arrayValue};
    for (auto &[id, waypoint] : ast.nodes) {
        waypoint_list.append(Json::objectValue);
        auto &last = waypoint_list[waypoint_list.size() - 1];
        last["id"] = Json::Value{static_cast<int>(id)};
        last["x"] = waypoint.translation.x;
        last["y"] = waypoint.translation.z;
        last["type"] = to_string(waypoint.waypointType);
        last["adjList"] = Json::Value{Json::arrayValue};
        std::for_each(std::begin(waypoint.adjlist), std::end(waypoint.adjlist),
                      [&last](int adj) { last["adjList"].append(adj); });
    }
    static_config.set("waypoints", waypoint_list);
}

void robot::Orchestrator::load_webots_to_config(const std::filesystem::path &input_file)
{
    std::ifstream infile(input_file);
    if (!infile.is_open()) {
        std::cerr << "The file " << input_file << " could not be opened.\n";
        exit(1);
    }
    Parser parser = Parser(infile);
    AST ast = parser.parse_stream();

    if (ast.nodes.size() == 0) {
        throw MalformedWorldFileError{"No waypoints found"};
    }

    std::vector<int> stations;
    std::vector<int> end_stations;
    std::vector<int> vias;
    for (auto &[id, waypoint] : ast.nodes) {
        switch (waypoint.waypointType) {
        case WaypointType::eStation:
            stations.push_back(id);
            break;
        case WaypointType::eEndPoint:
            end_stations.push_back(id);
            break;
        case WaypointType::eVia:
            vias.push_back(id);
            break;
        default:
            throw std::runtime_error{""};
        }
    }

    // Get number of stations, endpoints and waypoint
    int station_count = stations.size(), endpoint_count = end_stations.size(),
        via_count = vias.size(), waypoint_count = station_count + endpoint_count + via_count;

    static_config.set("stations", stations);
    static_config.set("end_stations", end_stations);
    static_config.set("vias", vias);
    static_config.set("number_of_stations", station_count);
    static_config.set("number_of_end_stations", endpoint_count);
    static_config.set("number_of_vias", via_count);
    static_config.set("number_of_waypoints", waypoint_count);
    static_config.set("number_of_robots", parser.number_of_robots);

    add_waypoint_matrix(ast, waypoint_count);
    add_station_matrix(ast, waypoint_count);
    dump_waypoint_info(ast);
}

void robot::Orchestrator::request_broadcast_info()
{
    broadcast_client.send("get_robot_locations");
}

void robot::Orchestrator::send_robot_info(int robot_id, const robot::Info &robot_info)
{
    broadcast_client.send("post_robot_location, " + robot_info.to_json().toStyledString());
}

std::string robot::Orchestrator::recv_broadcast_info()
{
    // Gets the latest info from the broadcaster
    return broadcast_client.receive_blocking();
}

void robot::Orchestrator::write_static_config(const std::filesystem::path &path)
{
    static_config.write_to_file(path.c_str());
}

void robot::Orchestrator::write_dynamic_config(const std::filesystem::path &path)
{
    dynamic_config.write_to_file(path.c_str());
}
