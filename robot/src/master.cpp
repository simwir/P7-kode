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

    load_webots_to_config();
    write_static_config();

    station_subscriber = std::make_shared<AsyncStationSubscriber>(stations);
    waypoint_subscriber = std::make_shared<AsyncWaypointSubscriber>(waypoints);
    eta_subscriber = std::make_shared<AsyncEtaSubscriber>();

    station_scheduler.add_subscriber(station_subscriber->shared_from_this());
    waypoint_scheduler.add_subscriber(waypoint_subscriber->shared_from_this());
    eta_extractor.add_subscriber(eta_subscriber->shared_from_this());

    // Connecting to the Port Discovery Service
    tcp::Client PDSClient{robot_host, PORT_TO_PDS};
    PDSClient.send("get_robot," + std::to_string(robot_id));
    port_to_controller = PDSClient.receive_blocking();
    std::cerr << port_to_controller << std::endl;
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
    stations.clear();
    end_stations.clear();
    vias.clear();
    waypoints.clear();

    for (auto &[id, waypoint] : ast.nodes) {
        waypoints.push_back(id);
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
    static_config.set("station_delay", STATION_DELAY);
    static_config.set("waypoint_delay", WAYPOINT_DELAY);
    static_config.set("uncertainty", UNCERTAINTY);
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
    current_state.station_plan = station_subscriber->get();
    current_state.waypoint_plan = waypoint_subscriber->get();
    current_state.location = controller_state.position;
    last_update_time = current_time;
    broadcast_client.send("put_robot_info," + current_state.to_json().toStyledString());
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
    dynamic_config.set(DESTINATION, waypoint);
}

void robot::Master::main_loop()
{

    // Bootstrap route
    get_dynamic_state();
    dynamic_config.set(NEXT_STATION, get_closest_waypoint([](auto wp) {
                           return wp.waypointType == WaypointType::eStation;
                       }));
    write_dynamic_config();
    std::cerr << "scheduling station\n";
    station_scheduler.start();
    std::cerr << "started scheduling station\n";
    station_scheduler.wait_for_result();
    std::cerr << "done scheduling station\n";

    get_dynamic_state();

    dynamic_config.set(DESTINATION, station_subscriber->read().at(0));
    dynamic_config.set(NEXT_WAYPOINT, get_closest_waypoint([](auto) { return true; }));
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
        current_time = get_webots_time();
        get_dynamic_state();
        std::cerr << "loop\n";

        // if station schedule invalidated or new station schedule
        //    then reschedule waypoints
        if (station_subscriber->is_dirty()) {
            got_fresh_info = true;
            // TODO save station schedule to file
            // Assuming that there will not be two HOLD instructions in a row.
            scheduling::Action act = waypoint_subscriber->get().front();
            if (act.type == scheduling::ActionType::Hold) {
                act = waypoint_subscriber->get().at(1);
            }
            dynamic_config.set(NEXT_WAYPOINT, act.value);
            dynamic_config.set(DESTINATION, station_subscriber->read().at(0));
            waypoint_scheduler.start();
        }

        if (waypoint_subscriber->is_dirty()) {
            got_fresh_info = true;
            current_state.waypoint_plan = waypoint_subscriber->read();
            eta_extractor.start();
        }

        // if new eta report ready
        //    then broadcast it minus time elapsed
        if (eta_subscriber->is_dirty()) {
            got_fresh_info = true;
            double time_delta = current_time - eta_start_time;
            double current_eta = eta_subscriber->get() - time_delta;
            current_state.eta = current_eta;
            dynamic_config.set("station_eta", current_eta);
        }

        // if at waypoint
        //    then tell robot of next waypoint;
        //         abort waypoint scheduling; start new one
        if (controller_state.is_stopped && current_time > hold_until) {
            got_fresh_info = true;
            // TODO broadcast position info

            if (waypoint_subscriber->get().empty()) {
                std::cerr << "NOTE: waiting for waypoint result" << std::endl;
                waypoint_scheduler.wait_for_result();
            }

            scheduling::Action current_waypoint = waypoint_subscriber->get().front();
            waypoint_subscriber->pop();
            scheduling::Action next_waypoint = waypoint_subscriber->get().front();
            if (current_waypoint.type != scheduling::ActionType::Hold) {
                visited_waypoints.push_back(current_waypoint.value);
            }

            auto is_station = [&](int id) {
                return ast.nodes.at(id).waypointType == WaypointType::eStation;
            };

            // hold for n units in webots time
            if (next_waypoint.type == scheduling::ActionType::Hold) {
                hold_until = current_time + next_waypoint.value;
            }
            else {
                set_robot_destination(next_waypoint.value);
            }

            // if committed to station dest or at station
            //    then reschedule stations
            if (is_station(next_waypoint.value) || is_station(current_waypoint.value)) {
                // previous waypoint scheduling is obsolete since we're at/going to a station
                waypoint_scheduler.abort();
                visited_waypoints.clear();

                // setup input for station scheduling.
                set_station_visited(next_waypoint.value);

                dynamic_config.set("stations_to_visit", order);
                write_dynamic_config();
                station_scheduler.start();
            }
            else {
                dynamic_config.set("visited_waypoints", visited_waypoints);

                write_dynamic_config();
                waypoint_scheduler.start();
            }
        }
        // send update if there are new things to report
        if (got_fresh_info || current_time - last_update_time >= UPDATE_INTERVAL_MS) {
            // TODO Maybe update ETA based on time elapsed since last update.
            // Not an immediately trivial change since ETAs might become negative.
            send_robot_info();
            std::cerr << "writing dynamic config" << std::endl;
            write_dynamic_config();
        }
    }
}

int robot::Master::get_webots_time()
{
    auto msg = webots_clock_client->atomic_blocking_request("get_time");
    try {
        return std::stoi(msg);
    }
    catch (std::invalid_argument &e) {
        throw std::logic_error{"Error: couldn't parse time from " + msg};
    }
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
