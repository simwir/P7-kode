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
#define PORT_TO_WALL_CLOCK "5555"

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
    PDSClient.send("get_robot," + std::to_string(robot_id));
    port_to_controller = PDSClient.receive_blocking();
    // TODO handle/report error if PDS does not know a port yet

    // Connecting to the WeBots Controller
    webot_client = std::make_unique<tcp::Client>(robot_host, port_to_controller);
    webots_clock_client = std::make_unique<tcp::Client>("127.0.0.1", PORT_TO_WALL_CLOCK);
    current_state.id = id;
}

void robot::Master::load_webots_to_config()
{
    ast = webots_parser.parse_stream();

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

    static_config.set(STATIONS, stations);
    static_config.set(END_STATIONS, end_stations);
    static_config.set(VIAS, vias);
    static_config.set(NUMBER_OF_STATIONS, station_count);
    static_config.set(NUMBER_OF_END_STATIONS, endpoint_count);
    static_config.set(NUMBER_OF_VIAS, via_count);
    static_config.set(NUMBER_OF_WAYPOINTS, number_of_waypoints);
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

void robot::Master::send_robot_info()
{
    current_state.station_plan = std::vector<int>{std::begin(station_subscriber->get()),
                                                  std::end(station_subscriber->get())};
    current_state.waypoint_plan = std::vector<scheduling::Action>{
        std::begin(waypoint_subscriber->get()), std::end(waypoint_subscriber->get())};
    current_state.location = controller_state.position;
    broadcast_client.send("post_robot_info," + current_state.to_json().toStyledString());
}

std::string robot::Master::receive_broadcast_info()
{
    return broadcast_client.receive_blocking();
}

std::string robot::Master::receive_controller_info()
{
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

    dynamic_config.set(ROBOT_INFO_MAP, robot_info.to_json());
    dynamic_config.set(SELF_STATE, controller_state.to_json());
}

void robot::Master::update_dynamic_state()
{
    get_dynamic_state();
    write_dynamic_config();
}

void robot::Master::write_static_config()
{
    static_config.write_to_file(static_conf);
}

void robot::Master::write_dynamic_config()
{
    dynamic_config.write_to_file(dynamic_conf);
}

void robot::Master::set_robot_destination(int waypoint)
{
    Translation point = ast.nodes.at(waypoint).translation;
    webot_client->send("set_destination," + std::to_string(point.x) + "," +
                       std::to_string(point.z));
}

void robot::Master::main_loop()
{
    load_webots_to_config();
    write_static_config();

    station_scheduler.add_subscriber(station_subscriber->shared_from_this());
    waypoint_scheduler.add_subscriber(waypoint_subscriber->shared_from_this());
    eta_extractor.add_subscriber(eta_subscriber->shared_from_this());

    // Bootstrap route
    get_dynamic_state();
    dynamic_config.set(CURRENT_STATION, get_closest_waypoint([](auto wp) {
                           return wp.waypointType == WaypointType::eStation;
                       }));
    write_dynamic_config();
    std::cerr << "scheduling station\n";
    station_scheduler.start();
    std::cerr << "started scheduling station\n";
    station_scheduler.wait_for_result();
    std::cerr << "done scheduling station\n";

    get_dynamic_state();

    dynamic_config.set(DESTINATION, station_subscriber->get().at(0));
    dynamic_config.set(CURRENT_WAYPOINT, get_closest_waypoint([](auto) { return true; }));
    write_dynamic_config();
    std::cerr << "scheduling waypoint\n";
    waypoint_scheduler.start();
    std::cerr << "started scheduling waypoint\n";
    waypoint_scheduler.wait_for_result();
    std::cerr << "done scheduling waypoint\n";
    send_robot_info();

    running = true;
    while (running) {
        bool got_fresh_info = false;
        current_webots_time = get_webots_time();
        std::cerr << "loop\n";

        // if station schedule invalidated or new station schedule
        //    then reschedule waypoints
        if (station_subscriber->is_dirty()) {
            got_fresh_info = true;
            station_subscriber->get();
            // TODO save station schedule to file
            // Assuming that there will not be two HOLD instructions in a row.
            scheduling::Action act = waypoint_subscriber->get().front();
            if (act.type == scheduling::ActionType::Hold) {
                act = waypoint_subscriber->get().at(1);
            }
            dynamic_config.set(CURRENT_WAYPOINT, act.value);
            dynamic_config.set(DESTINATION, station_subscriber->get().at(0));
            waypoint_scheduler.start();
        }

        if (waypoint_subscriber->is_dirty()) {
            got_fresh_info = true;
            // TODO set robot location from waypoint schedule
            // TODO broadcast waypoint schedule
        }

        // if new eta report ready
        //    then broadcast it minus time elapsed
        if (eta_subscriber->is_dirty()) {
            got_fresh_info = true;
            double time_delta = current_webots_time - eta_start_time;
            double current_eta = eta_subscriber->get() - time_delta;
            current_state.eta = current_eta;
        }

        // if at waypoint
        //    then tell robot of next waypoint;
        //         abort waypoint scheduling; start new one
        if (controller_state.is_stopped && current_webots_time > hold_untill) {
            got_fresh_info = true;
            // TODO broadcast position info

            if (waypoint_subscriber->get().empty()) {
                waypoint_scheduler.wait_for_result();
            }

            scheduling::Action current_waypoint = waypoint_subscriber->get().front();
            waypoint_subscriber->get().pop_front();
            scheduling::Action next_waypoint = waypoint_subscriber->get().front();

            auto is_station = [&](int id) {
                return ast.nodes.at(id).waypointType == WaypointType::eStation;
            };

            // hold for n units in webots time
            if (next_waypoint.type == scheduling::ActionType::Hold) {
                hold_untill = current_webots_time + next_waypoint.value;
            }
            else {
                set_robot_destination(next_waypoint.value);
            }

            // if committed to station dest or at station
            //    then reschedule stations
            if (is_station(next_waypoint.value) || is_station(current_waypoint.value)) {
                // TODO send state s.t. station scheduler starts at actual station
                waypoint_scheduler.abort();
                station_scheduler.start();
            }
            else {
                waypoint_scheduler.start();
            }
        }
        // TODO maybe just broadcast anyway after a while?
        if (got_fresh_info) {
            send_robot_info();
            std::cerr << "writing dynamic config" << std::endl;
            write_dynamic_config();
        }
    }
}

double robot::Master::get_webots_time()
{
    webots_clock_client->send("get_time");
    auto msg = webots_clock_client->receive_blocking();
    std::stringstream ss{msg};
    double time;
    ss >> time;
    if (!(ss.good() || ss.eof())) {
        throw std::logic_error{"Error: couldn't parse time from " + msg};
    }
    return time;
}

int robot::Master::get_closest_waypoint(std::function<bool(Waypoint)> pred)
{
    int best_wp = std::numeric_limits<int>::max();
    double best_dist = std::numeric_limits<double>::max();
    for (auto &[id, wp] : ast.nodes) {
        if (pred(wp)) {
            auto &[x, _, y] = wp.translation;
            auto dist = euclidean_distance(Point{x, y}, controller_state.position);
            if (dist < best_dist) {
                best_dist = dist;
                best_wp = id;
            }
        }
    }
    return best_wp;
}
