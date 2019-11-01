#include<fstream>

#include "master.hpp"
#include "config.hpp"
#include "location.hpp"
#include "../tcp/include/tcp/client.hpp"
#include "../wbt-translator/webots_parser.hpp"
#include "../wbt-translator/apsp.hpp"
#include "../wbt-translator/distance_matrix.hpp"

#define PORT_TO_BROADCASTER "5435"
#define PORT_TO_PDS "4444"

robot::Master::Master(std::string robot_host, std::string broadcast_host ,int robot_id)
        : broadcast_client(broadcast_host, PORT_TO_BROADCASTER) {
    std::string port_to_controller;
    std::vector<std::string> recieved_strings;

    //Connecting to the Port Discovery Service
    tcp::Client PDSClient = tcp::Client(robot_host, PORT_TO_PDS); 
    PDSClient.send("get_robot," + robot_id);
    do{
        recieved_strings = PDSClient.receive();
    }while(recieved_strings.size == 0);
    if (recieved_strings.size == 1)
    {
        port_to_controller = recieved_strings[0];
    }
    else{
        throw robot::RecievedMessageException("Recieved" + recieved_strings.size + "messages while only one was expected.");
    }
    
    //Connecting to the WeBots Controller
    webot_client = std::make_unique<tcp::Client>(robot_host, port_to_controller);
}

void robot::Master::load_webots_to_config(std::string input_file, std::string output_file){
    std::ifstream infile(input_file);
    if (!infile.is_open()) {
        std::cerr << "The file " << input_file << " could not be opened.\n";
        exit(1);
    }
    Parser parser = Parser(infile);
    AST ast = parser.parse_stream();

    if (ast.nodes.size() == 0) {
        std::cerr << "Malformed world file. No waypoints found.";
        exit(1);
    }

    //Get distance matrix for waypoints
    distance_matrix waypoint_matrix = distance_matrix(ast);

    //Get distance matrix for stations
    apsp_result station_matrix = all_pairs_shortest_path(ast);

    //Get number of stations, endpoints and waypoint
    int station_count = 0, endpoint_count = 0, waypoint_count = 0;

    for (auto &[id, waypoint] : ast.nodes){
        switch (waypoint.waypointType)
        {
        case WaypointType::eStation:
            station_count++;
            break;
        case WaypointType::eEndPoint:
            endpoint_count++;
            break;
        case WaypointType::eVia:
            waypoint_count++;
        default:
            throw MalformedWorldFileError("Nodes of neither station, endpoint or waypoint in world.");
            break;
        }
    }

    config.set<int>("number_of_stations", station_count);
    config.set<int>("number_of_end_stations", endpoint_count);
    config.set<int>("number_of_waypoints", waypoint_count);
    config.set<int>("number_of_robots", parser.number_of_robots);
}

void robot::Master::request_broadcast_info(){
    broadcast_client.send("get_robot_locations"); 
}

void robot::Master::send_robot_info(int robot_id, Location location){
    robot::LocationMap map = robot::LocationMap();
    map.locations.insert({robot_id, location});
    broadcast_client.send("post_robot_location, " + location.);
}

std::string robot::Master::recv_broadcast_info(){

}