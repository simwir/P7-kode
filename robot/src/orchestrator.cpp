/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler
 *Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 *associated documentation files (the "Software"), to deal in the Software without restriction,
 *including without limitation the rights to use, copy, modify, merge, publish, distribute,
 *sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or
 *substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 *NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
 *OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <fstream>

#include "config/config.hpp"
#include "robot/info.hpp"
#include "robot/orchestrator.hpp"
#include "tcp/client.hpp"
#include "wbt-translator/apsp.hpp"
#include "wbt-translator/distance_matrix.hpp"
#include "wbt-translator/webots_parser.hpp"

robot::Orchestrator::Orchestrator(int robot_id, std::istream &world_file, NetworkInfo network_info)
    : id(robot_id), network_info(network_info),
      com_module(network_info.com_addr, network_info.com_port), webots_parser(world_file)
{
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
    tcp::Client PDSClient{network_info.pds_addr, network_info.pds_port};
    PDSClient.send("get_robot," + std::to_string(robot_id));
    std::string port_to_controller = PDSClient.receive_blocking();
    std::cerr << port_to_controller << std::endl;
    // crash and burn if the port is not valid (such as if there is no port).
    tcp::validate_port_format(port_to_controller);

    // Connecting to the WeBots Controller
    robot_client = std::make_unique<tcp::Client>(network_info.robot_addr, port_to_controller);
    clock_client =
        std::make_unique<robot::WebotsClock>(network_info.time_addr, network_info.time_port);
    current_state.id = id;
}

void robot::Orchestrator::load_webots_to_config()
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
        }
    }

    static_config.set(STATIONS, stations);
    static_config.set(END_STATIONS, end_stations);
    static_config.set(VIAS, vias);

    // TODO move to static config with help from broadcaster
    // static_config.set("number_of_robots", webots_parser.number_of_robots);

    add_waypoint_matrix(ast);
    add_station_matrix(ast);
    dump_waypoint_info(ast);
    static_config.set("station_delay", STATION_DELAY);
    static_config.set("waypoint_delay", WAYPOINT_DELAY);
    static_config.set("uncertainty", UNCERTAINTY);
}

void robot::Orchestrator::add_waypoint_matrix(const AST &ast)
{
    // Get distance matrix for waypoints
    std::vector<std::vector<double>> waypoint_matrix = distance_matrix{ast}.get_data();

    // Convert waypoint distance matrix.
    Json::Value jsonarray_waypoint_matrix{Json::arrayValue};
    for (size_t i = 0; i < ast.num_waypoints(); i++) {
        Json::Value jsonarray_waypoint_row{Json::arrayValue};
        for (size_t j = 0; j < ast.num_waypoints(); j++) {
            jsonarray_waypoint_row.append(waypoint_matrix.at(i).at(j));
        }
        jsonarray_waypoint_matrix.append(jsonarray_waypoint_row);
    }
    static_config.set("waypoint_distance_matrix", jsonarray_waypoint_matrix);
}

void robot::Orchestrator::add_station_matrix(const AST &ast)
{
    // Get distance matrix for stations
    std::map<int, std::map<int, double>> apsp_distances = all_pairs_shortest_path(ast).dist;

    // Convert shortest paths between stations
    Json::Value jsonarray_apsp_distances{Json::arrayValue};
    for (size_t i = 0; i < ast.num_waypoints(); i++) {
        if (ast.nodes.at(i).waypointType == WaypointType::eStation ||
            ast.nodes.at(i).waypointType == WaypointType::eEndPoint) {
            Json::Value jsonarray_apsp_row{Json::arrayValue};
            for (size_t j = 0; j < ast.num_waypoints(); j++) {
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

void robot::Orchestrator::request_robot_info()
{
    com_module.send("get_robot_info");
}

void robot::Orchestrator::request_controller_info()
{
    robot_client->send("get_state");
}

void robot::Orchestrator::send_robot_info()
{
    current_state.station_plan = station_subscriber->get();
    current_state.waypoint_plan = waypoint_subscriber->get();
    current_state.location = controller_state.position;
    last_update_time = current_time;
    com_module.send("put_robot_info," + current_state.to_json().toStyledString());
}

std::string robot::Orchestrator::receive_robot_info()
{
    return com_module.receive_blocking();
}

void robot::Orchestrator::get_new_order()
{
    // order_service_client->send("get_order");
    // TODO temp until better API on order generator
    order_service_client->send(std::to_string(rand() % 8));
    auto response = order_service_client->receive_blocking();
    std::stringstream ss{response};
    Json::Value val;
    ss >> val;
    std::vector<int> order;
    for (const auto &station : val) {
        order.push_back(station.asInt());
        std::cerr << order.back() << ", ";
    }
    std::cerr << std::endl;
    this->order = std::move(order);
}

std::string robot::Orchestrator::receive_controller_info()
{
    return robot_client->receive_blocking();
}

void robot::Orchestrator::get_dynamic_state()
{
    std::cerr << "getting dynamic state\n";
    request_robot_info();
    request_controller_info();
    auto broadcast_info = receive_robot_info();
    auto _controller_state = receive_controller_info();
    robot_info = robot::InfoMap::from_json(broadcast_info);
    controller_state = robot::ControllerState::parse(_controller_state);

    dynamic_config.set(ROBOT_INFO_MAP, robot_info.to_json());
    dynamic_config.set(SELF_STATE, controller_state.to_json());
}

void robot::Orchestrator::update_dynamic_state()
{
    get_dynamic_state();
    write_dynamic_config();
}

void robot::Orchestrator::write_static_config()
{
    static_config.write_to_file(static_conf);
}

void robot::Orchestrator::write_dynamic_config()
{
    dynamic_config.write_to_file(dynamic_conf);
}

void robot::Orchestrator::set_robot_destination(int waypoint)
{
    Translation point = ast.nodes.at(waypoint).translation;
    robot_client->send("set_destination," + std::to_string(point.x) + "," +
                       std::to_string(point.z));
    dynamic_config.set(DESTINATION, waypoint);
}

void robot::Orchestrator::main_loop()
{
    srand(std::time(NULL));
    // Bootstrap route
    get_dynamic_state();
    // set next station to our closest station during the bootstrapping (before any actual schedule
    // exists).
    get_new_order();
    dynamic_config.set("stations_to_visit", order);
    dynamic_config.set(NEXT_STATION, get_closest_waypoint([](auto wp) {
                           return wp.waypointType == WaypointType::eStation;
                       }));
    dynamic_config.set(STATION_ETA, 0);
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
        current_time = clock_client->get_current_time();
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
            dynamic_config.set(STATION_ETA, current_eta);
        }

        // if at waypoint
        //    then tell robot about next waypoint;
        //         abort waypoint scheduling; start new one
        if (controller_state.is_stopped && current_time > hold_until) {
            got_fresh_info = true;

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

            // hold for n units in webots time
            if (next_waypoint.type == scheduling::ActionType::Hold) {
                hold_until = current_time + next_waypoint.value;
            }
            else {
                set_robot_destination(next_waypoint.value);
            }

            auto is_station = [&](int id) {
                return ast.nodes.at(id).waypointType == WaypointType::eStation;
            };
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

int robot::Orchestrator::get_closest_waypoint(std::function<bool(Waypoint)> pred)
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

void robot::Orchestrator::set_station_visited(int station)
{
    if (auto it = std::find(order.begin(), order.end(), station); it != order.end()) {
        order.erase(it);
    }
    visited_waypoints.push_back(station);
}
