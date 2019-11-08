#include <fstream>

#include "robot/config.hpp"
#include "robot/info.hpp"
#include "robot/master.hpp"
#include "tcp/client.hpp"
#include "wbt-translator/apsp.hpp"
#include "wbt-translator/distance_matrix.hpp"
#include "wbt-translator/webots_parser.hpp"

#define PORT_TO_BROADCASTER "5435"
#define PORT_TO_PDS "4444"

std::optional<Json::Value> parse(const std::string s)
{
    Json::Value value;
    std::stringstream ss{s};
    try {
        ss >> value;
        return {value};
    }
    catch (std::exception &e) {
        return std::nullopt;
    }
}

robot::Master::Master(const std::string &robot_host, const std::string &broadcast_host,
                      int robot_id, std::istream &world_file)
    : id(robot_id), broadcast_client(broadcast_host, PORT_TO_BROADCASTER), webots_parser(world_file)
{
    std::string port_to_controller;
    std::string recieved_string;

    station_subscriber = std::make_shared<AsyncStationSubscriber>();
    waypoint_subscriber = std::make_shared<AsyncWaypointSubscriber>();
    eta_subscriber = std::make_shared<AsyncEtaSubscriber>();

    // Connecting to the Port Discovery Service
    tcp::Client PDSClient{robot_host, PORT_TO_PDS};
    PDSClient.send("get_robot_info," + std::to_string(robot_id));
    port_to_controller = PDSClient.receive_blocking();

    // Connecting to the WeBots Controller
    // webot_client = std::make_unique<tcp::Client>(robot_host, port_to_controller);
}

void robot::Master::load_webots_to_config()
{
    AST ast = webots_parser.parse_stream();

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
        via_count = vias.size();
    int number_of_waypoints = station_count + endpoint_count + via_count;

    static_config.set("stations", stations);
    static_config.set("end_stations", end_stations);
    static_config.set("vias", vias);
    static_config.set("number_of_stations", station_count);
    static_config.set("number_of_end_stations", endpoint_count);
    static_config.set("number_of_vias", via_count);
    static_config.set("number_of_waypoints", number_of_waypoints);
    // TODO move to static config with help from broadcaster
    // static_config.set("number_of_robots", webots_parser.number_of_robots);

    // Get distance matrix for waypoints
    std::vector<std::vector<double>> waypoint_matrix = distance_matrix{ast}.get_data();

    // Get distance matrix for stations
    std::map<int, std::map<int, double>> apsp_distances = all_pairs_shortest_path(ast).dist;

    // Flatten waypoint distance matrix.
    Json::Value jsonarray_waypoint_matrix{Json::arrayValue};
    size_t columns = waypoint_matrix.front().size();
    size_t rows = waypoint_matrix.size();
    for (size_t i = 0; i < rows; i++) {
        for (size_t h = 0; h < columns; h++) {
            jsonarray_waypoint_matrix.append(waypoint_matrix[i][h]);
        }
    }
    static_config.set("waypoint_distance_matrix", jsonarray_waypoint_matrix);

    // Flatten shortest paths between stations
    Json::Value jsonarray_apsp_distances{Json::arrayValue};
    columns = apsp_distances.at(1).size();
    rows = apsp_distances.size();
    for (int i = 0; i < number_of_waypoints; i++) {
        for (int h = 0; h < number_of_waypoints; h++) {
            if (ast.nodes.at(i).waypointType == WaypointType::eStation &&
                ast.nodes.at(h).waypointType == WaypointType::eStation) {
                jsonarray_apsp_distances.append(apsp_distances.at(i).at(h));
            }
        }
    }

    // Dump all waypoint information.
    Json::Value waypoint_list{Json::arrayValue};
    for (auto &[id, waypoint] : ast.nodes) {
        waypoint_list.append(Json::objectValue);
        auto &last = waypoint_list[waypoint_list.size() - 1];
        last["id"] = id;
        last["x"] = waypoint.translation.x;
        last["y"] = waypoint.translation.z;
        last["type"] = to_string(waypoint.waypointType);
        last["adjList"] = Json::Value{Json::arrayValue};
        std::for_each(std::begin(waypoint.adjlist), std::end(waypoint.adjlist),
                      [&last](int adj) { last["adjList"].append(adj); });
    }
    static_config.set("waypoints", waypoint_list);
    static_config.set("station_distance_matrix", jsonarray_apsp_distances);
}

void robot::Master::request_broadcast_info()
{
    broadcast_client.send("get_robot_info");
}

void robot::Master::request_controller_info()
{
    webot_client->send("get_state");
}

void robot::Master::send_robot_info(const robot::Info &robot_info)
{
    broadcast_client.send("post_robot_location, " + robot_info.to_json().toStyledString());
}

std::string robot::Master::receive_broadcast_info()
{
    return broadcast_client.receive_blocking();
}
 
std::string robot::Master::receive_controller_info()
{
    webot_client->send("get_state");
    return webot_client->receive_blocking();
}

void robot::Master::get_dynamic_state()
{
    std::cerr << "getting dynamic state\n";
    request_broadcast_info();
    request_controller_info();
    auto broadcast_info = receive_broadcast_info();
    auto _controller_state = receive_controller_info();
    robot_info = robot::InfoMap::from_json(broadcast_info);
    controller_state = robot::parse_controller_state(_controller_state);
    // TODO write schedules to config

    // TODO wait for broadcaster merge
    // dynamic_config.set("robot_info", robot_info.to_json());
    dynamic_config.set("our_state", controller_state.to_json());
}

void robot::Master::write_static_config(const std::filesystem::path &path)
{
    static_config.write_to_file(path.c_str());
}

void robot::Master::write_dynamic_config(const std::filesystem::path &path)
{
    dynamic_config.write_to_file(path.c_str());
}

void robot::Master::main_loop()
{
    load_webots_to_config();
    write_static_config("static_conf.json");

    station_scheduler.add_subscriber(station_subscriber->shared_from_this());
    waypoint_scheduler.add_subscriber(waypoint_subscriber->shared_from_this());
    eta_extractor.add_subscriber(eta_subscriber->shared_from_this());

    // Bootstrap route
    get_dynamic_state();
    std::cerr << "scheduling station\n";
    station_scheduler.start();
    std::cerr << "started scheduling station\n";
    station_scheduler.wait_for_result();
    std::cerr << "done scheduling station\n";
    // TODO Broadcast station schedule

    get_dynamic_state();
    std::cerr << "scheduling waypoint\n";
    waypoint_scheduler.start();
    std::cerr << "started scheduling waypoint\n";
    waypoint_scheduler.wait_for_result();
    std::cerr << "done scheduling waypoint\n";
    // TODO broadcast waypoint schedule

    while (running) {
        std::cerr << "loop\n";

        // if station schedule invalidated or new station schedule
        //    then reschedule waypoints
        if (station_subscriber->is_dirty()) {
            // TODO broadcast station schedule
            // TODO save station schedule to file
            waypoint_scheduler.start();
        }

        if (waypoint_subscriber->is_dirty()) {
            // TODO set robot location from waypoint schedule
            // TODO broadcast waypoint schedule
        }

        if (eta_subscriber->is_dirty()) {
            // TODO adjust eta value for time delta
            // TODO broadcast eta
            double current_time = get_webots_time();
            double time_delta = current_time - last_webots_time;

            broadcast_eta(eta_subscriber->get() - time_delta);
        }

        // if at waypoint
        //    then tell robot of next waypoint;
        //         abort waypoint scheduling; start new one
        if (controller_state.is_stopped) {
            // TODO send new destination to robot
            // TODO broadcast position info

            // TODO get actual index into waypoint schedule

            // scaffolding variables and lambda. TODO cleanup
            auto next_waypoint = waypoint_subscriber->get().at(0);
            auto current_waypoint = next_waypoint;
            auto is_station = [](auto &&) { return true; };

            waypoint_scheduler.start();

            // if committed to station dest or at station
            //    then reschedule stations
            if (is_station(next_waypoint) || is_station(current_waypoint)) {
                // TODO send state s.t. station scheduler starts at actual station
                waypoint_scheduler.abort();
                station_scheduler.start();
            }
        }

        // receive information from broadcaster and controller
        // if new station or waypoint schedule ready
        //    then broadcast it
        // if new eta report ready
        //    then broadcast it minus time elapsed
        // if at waypoint
        //    then tell robot of next waypoint;
        //         abort waypoint scheduling; start new one
        // if committed to station dest or at station
        //    then reschedule stations
    }
}

double robot::Master::get_webots_time()
{
    webots_clock_client->send("get_time");
    auto msg = webots_clock_client->receive_blocking();
    return stod(msg);
}

void robot::Master::broadcast_eta(double eta)
{
}
