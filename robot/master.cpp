#include<fstream>

#include "master.hpp"
#include "../tcp/client.hpp"
#include "../wbt-translator/webots_parser.hpp"
#include "../wbt-translator/apsp.hpp"

robot::Master::Master(){
    //TODO: Port discovery service port: 4444. Connect to this to get port for controller
    //TODO: Create clients to controller and broadcaster
    //tcp::Client client_webots = tcp::Client(host, port);
}

void robot::Master::load_webots_to_config(std::string input_file){
    std::ifstream infile(input_file);
    AST ast = Parser(infile).parse_stream();

    if (ast.nodes.size() == 0) {
        std::cerr << "Malformed world file. No waypoints found.";
        exit(1);
    }

    std::map<int, std::map<int, double>> shortest_path;
    shortest_path = all_pairs_shortest_path(ast);
    std::string string_shortest_path = print_all_pairs_shortest_pairs(shortest_path);


}