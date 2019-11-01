#include <fstream>

#include "../tcp/include/tcp/client.hpp"
#include "../wbt-translator/apsp.hpp"
#include "../wbt-translator/distance_matrix.hpp"
#include "../wbt-translator/webots_parser.hpp"
#include "config.hpp"
#include "include/robot/info.hpp"
#include "master.hpp"

#define PORT_TO_BROADCASTER "5435"
#define PORT_TO_PDS "4444"

robot::Master::Master(const std::string &robot_host, const std::string &broadcast_host,
                      int robot_id)
    : broadcast_client(broadcast_host, PORT_TO_BROADCASTER)
{
    std::string port_to_controller;
    std::vector<std::string> recieved_strings;

    // Connecting to the Port Discovery Service
    tcp::Client PDSClient{robot_host, PORT_TO_PDS};
    PDSClient.send("get_robot," + robot_id);
    do {
        recieved_strings = PDSClient.receive();
    } while (recieved_strings.size() == 0);
    if (recieved_strings.size() == 1) {
        port_to_controller = recieved_strings[0];
    }
    else {
        throw robot::RecievedMessageException("Recieved" + recieved_strings.size +
                                              "messages while only one was expected.");
    }

    // Connecting to the WeBots Controller
    webot_client = std::make_unique<tcp::Client>(robot_host, port_to_controller);
}

void robot::Master::load_webots_to_config(std::filesystem::path input_file)
{
    std::ifstream infile(input_file);
    if (!infile.is_open()) {
        std::cerr << "The file " << input_file << " could not be opened.\n";
        exit(1);
    }
    Parser parser = Parser(infile);
    AST ast = parser.parse_stream();

    if (ast.nodes.size() == 0) {
        std::cerr << "Malformed world file. No waypoints found." << std::endl;
        exit(1);
    }

    // Get number of stations, endpoints and waypoint
    int station_count = 0, endpoint_count = 0, waypoint_count = 0;

    for (auto &[id, waypoint] : ast.nodes) {
        switch (waypoint.waypointType) {
        case WaypointType::eStation:
            station_count++;
            break;
        case WaypointType::eEndPoint:
            endpoint_count++;
            break;
        case WaypointType::eVia:
            waypoint_count++;
        default:
            throw MalformedWorldFileError(
                "Nodes of neither station, endpoint or waypoint in world.");
            break;
        }
    }

    config.set("number_of_stations", station_count);
    config.set("number_of_end_stations", endpoint_count);
    config.set("number_of_waypoints", waypoint_count);
    config.set("number_of_robots", parser.number_of_robots);

    // Get distance matrix for waypoints
    std::vector<std::vector<double>> waypoint_matrix = distance_matrix(ast).get_data;

    // Get distance matrix for stations
    std::map<int, std::map<int, double>> station_matrix = all_pairs_shortest_path(ast).dist;

    Json::Value jsonarray_waypoint_matrix{Json::arrayValue};
    size_t columns = waypoint_matrix.front().size();
    size_t rows = waypoint_matrix.size();
    size_t count = 0;
    for (std::size_t i = 0; i < rows; i++) {
        for (size_t h = 0; h < columns; h++) {
            jsonarray_waypoint_matrix[Json::ArrayIndex(count)] = waypoint_matrix[i][h];
            count++;
        }
    }

    config.set("waypoint_distance_matrix", jsonarray_waypoint_matrix);

    // TODO set station matrix
    Json::Value jsonarray_station_matrix{Json::arrayValue};
    count = 0;
    columns = station_matrix.at(1).size();
    rows = station_matrix.size();
    for (size_t i = 0; i < rows; i++) {
        for (size_t h = 0; h < columns; h++) {
            jsonarray_station_matrix[Json::ArrayIndex(count)] = station_matrix.at(i).at(h);
            count++;
        }
    }

    config.set("station_distance_matrix", jsonarray_station_matrix);
}

void robot::Master::request_broadcast_info()
{
    broadcast_client.send("get_robot_locations");
}

void robot::Master::send_robot_info(int robot_id, Info robot_info)
{
    broadcast_client.send("post_robot_location, " + robot_info.to_json);
}

std::string robot::Master::recv_broadcast_info()
{
    std::vector<std::string> strings_from_broadcaster;
    strings_from_broadcaster = broadcast_client.receive();

    // Gets the latest info from the broadcaster
    return strings_from_broadcaster.back();
}
