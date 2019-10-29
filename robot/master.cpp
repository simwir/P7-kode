#include<fstream>

#include "master.hpp"
#include "../tcp/include/client.hpp"
#include "../wbt-translator/webots_parser.hpp"
#include "../wbt-translator/apsp.hpp"
#include "../wbt-translator/distance_matrix.hpp"

robot::Master::Master(int robot_id){
    std::string port_to_controller;

    tcp::Client PDSClient = tcp::Client("localhost", "4444"); 
    PDSClient.send("get_robot," + robot_id);
    std::vector<std::string> recieved_strings = PDSClient.receive(0);
    if (recieved_strings.size == 1)
    {
        port_to_controller = recieved_strings[0];
    }
    else{
        // TODO: throw appropiate exception 
    }
    
    
    //TODO: Port discovery service port: 4444. Connect to this to get port for controller
    //TODO: Create clients to controller and broadcaster
    //tcp::Client client_webots = tcp::Client(host, port);
}

void robot::Master::load_webots_to_config(std::string input_file, std::string output_file){
    std::ifstream infile(input_file);
    if (!infile.is_open()) {
        std::cerr << "The file " << input_file << " could not be opened.\n";
        exit(1);
    }
    AST ast = Parser(infile).parse_stream();

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

    //TODO: Get number of robots


    


}