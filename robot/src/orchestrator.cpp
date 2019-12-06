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

#include "communication/info.hpp"
#include "config/config.hpp"
#include "robot/orchestrator.hpp"
#include "tcp/client.hpp"
#include "wbt-translator/apsp.hpp"
#include "wbt-translator/distance_matrix.hpp"
#include "wbt-translator/query_template_writer.hpp"
#include "wbt-translator/webots_parser.hpp"

#include <cassert>

#define TRACEME

#include "trace.def"

robot::Orchestrator::Orchestrator(int robot_id, std::istream &world_file, Options options)
    : id(robot_id), options(options), com_module(options.com_addr, options.com_port),
      webots_parser(world_file)
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
    tcp::Client PDSClient{options.pds_addr, options.pds_port};
    PDSClient.send("get_robot," + std::to_string(robot_id));
    std::string port_to_controller = PDSClient.receive_blocking();
    std::cerr << port_to_controller << std::endl;
    // crash and burn if the port is not valid (such as if there is no port).
    tcp::validate_port_format(port_to_controller);

    // Connecting to the WeBots Controller
    robot_client = std::make_unique<tcp::Client>(options.robot_addr, port_to_controller);
    clock_client = std::make_unique<robot::WebotsClock>(options.time_addr, options.time_port);
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

    auto is_station = [](Waypoint wp) {
        return wp.waypointType == WaypointType::eStation ||
               wp.waypointType == WaypointType::eEndPoint;
    };

    // Convert shortest paths between stations
    Json::Value jsonarray_apsp_distances{Json::arrayValue};
    for (size_t i = 0; i < ast.num_waypoints(); i++) {
        if (is_station(ast.nodes.at(i))) {
            Json::Value jsonarray_apsp_row{Json::arrayValue};
            for (size_t j = 0; j < ast.num_waypoints(); j++) {
                if (is_station(ast.nodes.at(j))) {
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

    tcp::Client order_service_client{options.order_addr, options.order_port};
    order_service_client.send("get_order");
    auto response = order_service_client.receive_blocking();
    order_rec_time = clock_client->get_current_time();
    std::stringstream ss{response};
    Json::Value val;
    ss >> val;
    std::vector<int> order;
    TRACE({
        std::cerr << "Orchestrator: order is ";
        for (const auto &station : val) {
            order.push_back(station.asInt());
            std::cerr << order.back() << ", ";
        }
    });
    TRACE(std::cerr << std::endl);
    dynamic_config.set("stations_to_visit", order);
    this->order = order;
}

std::string robot::Orchestrator::receive_controller_info()
{
    return robot_client->receive_blocking();
}

void robot::Orchestrator::get_dynamic_state()
{
    // std::cerr << "getting dynamic state\n";
    request_robot_info();
    request_controller_info();
    auto broadcast_info = receive_robot_info();
    auto _controller_state = receive_controller_info();
    robot_info = communication::InfoMap::from_json(broadcast_info);
    controller_state = robot::ControllerState::parse(_controller_state);
    robot_info.try_erase(this->id);
    if (robot_info.size() != last_num_robots) {
        create_query_file();
    }

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

void robot::Orchestrator::prepend_next_waypoint_to_schedule()
{
    auto old = waypoint_subscriber->get();
    old.insert(old.begin(), next_waypoint);
    waypoint_subscriber->reset(old);
}

void robot::Orchestrator::set_robot_destination(int waypoint)
{
    TRACE(std::cout << "Orchestrator: Sending robot to waypoint " << waypoint << std::endl);
    Translation point = ast.nodes.at(waypoint).translation;
    TRACE(std::cout << "Orchestrator: at coordinates (" << point.x << "," << point.z << ")"
                    << std::endl);
    robot_client->send("set_destination," + std::to_string(point.x) + "," +
                       std::to_string(point.z));
    dynamic_config.set(NEXT_WAYPOINT, waypoint);
}

void robot::Orchestrator::do_next_action()
{
    assert(!waypoint_subscriber->get().empty());
    // current_waypoint = waypoint_subscriber->get().front();
    current_state.waypoint_plan = waypoint_subscriber->get();
    current_waypoint = next_waypoint;
    waypoint_subscriber->pop();
    scheduling::Action next_action;
    if (!waypoint_subscriber->get().empty()) {
        next_action = waypoint_subscriber->get().front();
    }
    else {
        // When there's nothing left of the order and we are near an end station:
        // Get a new order and bootstrap new schedules for this order.
        if (order.empty() && waypoint_subscriber->get().empty() && is_end_station(next_waypoint)) {
            get_new_order();

            clear_visited_waypoints();
            write_dynamic_config();
            station_scheduler.start();
            station_scheduler.wait_for_result();

            next_station = station_subscriber->read().at(0);
            dynamic_config.set(NEXT_STATION, next_station);
            // fallthrough to waypoint scheduling afterward.
            write_dynamic_config();
        }

        if (station_scheduler.running()) {
            station_scheduler.wait_for_result();
            next_station = station_subscriber->read().at(0);
            dynamic_config.set(NEXT_STATION, next_station);
            write_dynamic_config();
        }

        waypoint_scheduler.maybe_start();
        waypoint_scheduler.wait_for_result();
        next_action = waypoint_subscriber->get().front();
    }

    assert(current_waypoint.type == scheduling::ActionType::Waypoint);
    visited_waypoints.push_back(current_waypoint.value);

    // hold for n units in webots time
    if (next_action.type == scheduling::ActionType::Hold) {
        hold_until = current_time + next_action.value * 1000;
        TRACE(std::cerr << "Orchestrator: holding until " << hold_until << std::endl);
    }
    else {
        next_waypoint = next_action;
        set_robot_destination(next_waypoint.value);
    }
    TRACE(std::cerr << "Orchestrator: current WP is \t" << current_waypoint << std::endl
                    << "Orchestrator: next WP is \t" << next_waypoint << std::endl);
    TRACE(std::cerr << "Orchestrator: plan is:\t");
    TRACE({
        auto sched = waypoint_subscriber->get();
        for (auto wp : sched) {
            std::cerr << wp << ' ';
        }
    });
    TRACE(std::cerr << std::endl);
}

std::optional<scheduling::Action> robot::Orchestrator::get_next_waypoint()
{
    for (size_t i = 0; i < waypoint_subscriber->get().size(); ++i) {
        if (auto wp = waypoint_subscriber->get().at(i);
            wp.type == scheduling::ActionType::Waypoint) {
            return wp;
        }
    }
    return std::nullopt;
}

void robot::Orchestrator::start_eta_calculation()
{
    eta_start_time = current_time;
    eta_extractor.start();
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
                           return wp.waypointType == WaypointType::eStation ||
                                  wp.waypointType == WaypointType::eEndPoint;
                       }));
    dynamic_config.set(STATION_ETA, 0);
    write_dynamic_config();
    TRACE(std::cerr << "Orchestrator: scheduling station\n");
    station_scheduler.start();
    TRACE(std::cerr << "Orchestrator: started scheduling station\n");
    station_scheduler.wait_for_result();
    TRACE(std::cerr << "Orchestrator: done scheduling station\n");

    get_dynamic_state();

    dynamic_config.set(NEXT_STATION, station_subscriber->read().at(0));
    current_waypoint = {scheduling::ActionType::Waypoint, get_closest_waypoint()};
    next_waypoint = current_waypoint;
    dynamic_config.set(NEXT_WAYPOINT, next_waypoint.value);
    dynamic_config.set(VISITED_WAYPOINTS, std::vector<int>{});

    create_query_file();
    write_dynamic_config();
    TRACE(std::cerr << "Orchestrator: scheduling waypoint\n");
    waypoint_scheduler.start();
    TRACE(std::cerr << "Orchestrator: started scheduling waypoint\n");
    waypoint_scheduler.wait_for_result();
    TRACE(std::cerr << "Orchestrator: done scheduling waypoint\n");
    send_robot_info();

    next_waypoint = get_next_waypoint().value();
    dynamic_config.set(NEXT_WAYPOINT, next_waypoint.value);
    prepend_next_waypoint_to_schedule();
    waypoint_subscriber->read();
    do_next_action();
    current_time = clock_client->get_current_time();
    start_eta_calculation();

    running = true;
    while (running) {
        bool got_fresh_info = false;
        // TODO optimize?
        create_query_file();
        current_time = clock_client->get_current_time();
        get_dynamic_state();

        // if station schedule invalidated or new station schedule
        //    then reschedule waypoints
        if (station_subscriber->is_dirty()) {
            got_fresh_info = true;
            // TODO save station schedule to file
            // Assuming that there will not be two HOLD instructions in a row.

            next_station = station_subscriber->read().at(0);
            dynamic_config.set(NEXT_STATION, next_station);
            write_dynamic_config();
            waypoint_scheduler.start();
        }

        if (waypoint_subscriber->is_dirty()) {
            got_fresh_info = true;
            current_state.waypoint_plan = waypoint_subscriber->read();
            start_eta_calculation();

            auto _next = get_next_waypoint();
            if (!_next.has_value() || _next->value != next_waypoint.value) {
                // prepend next waypoint to schedule since scheduling from next waypoint does not
                // include it.
                prepend_next_waypoint_to_schedule();
            }
        }

        // if new eta report ready
        //    then broadcast it minus time elapsed
        if (eta_subscriber->is_dirty()) {
            got_fresh_info = true;
            double time_delta = current_time - eta_start_time;
            double current_eta = eta_subscriber->read() - (time_delta / 1000.0);
            auto dist = dist_to_next_waypoint();
            TRACE(std::cerr << "dist: " << dist << '\t');
            current_eta += dist;
            TRACE(std::cerr << "time_delta: " << time_delta << "\tcurrent_eta: " << current_eta
                            << '\n');
            TRACE(std::cerr << "time_delta = " << current_time << " - " << eta_start_time
                            << std::endl);
            TRACE(std::cerr << "eta value: " << eta_subscriber->get() << std::endl);
            current_state.eta = current_eta;
        }

        // if at waypoint
        //    then tell robot about next waypoint;
        //         abort waypoint scheduling; start new one
        if (controller_state.is_stopped && current_time > hold_until) {
            got_fresh_info = true;
            // dist <= WAYPOINT_DIST_THRESHOLD
            if (waypoint_subscriber->get().empty()) {
                TRACE(std::cerr << "Orchestrator: waiting for waypoint result" << std::endl);
                waypoint_scheduler.wait_for_result();
            }

            int wp = get_closest_waypoint();
            if (wp != next_waypoint.value) {
                set_robot_destination(next_waypoint.value);
            }
            else {
                TRACE(std::cerr << "Orchestrator: expected at waypoint.\n");
                do_next_action();

                // if committed to station dest or at station
                //    then reschedule stations
                if (is_station(next_waypoint)) {
                    // previous waypoint scheduling is obsolete since we're at/going to a station
                    waypoint_scheduler.abort();
                    clear_visited_waypoints();

                    // setup input for station scheduling.
                    TRACE(std::cerr << "Orchestrator: visited waypoint " << next_waypoint.value
                                    << std::endl);
                    TRACE(std::cerr << "Orchestrator: from " << current_waypoint.value
                                    << std::endl);
                    set_station_visited(next_waypoint.value);
                    if (order.empty() && is_end_station(next_waypoint)) {
                    }
                    if (!order.empty()) {
                        dynamic_config.set("stations_to_visit", order);
                        write_dynamic_config();
                        station_scheduler.start();
                    }
                    else {
                        if (station_subscriber->get().empty()) {
                            std::cerr << "Orchestrator: Setting next station to closest endstation "
                                         "since schedule is somehow empty"
                                      << std::endl;
                            dynamic_config.set(NEXT_STATION, get_closest_waypoint([](auto wp) {
                                                   return wp.waypointType ==
                                                          WaypointType::eEndPoint;
                                               }));
                        }
                        else {
                            dynamic_config.set(NEXT_STATION, station_subscriber->get().front());
                        }
                        waypoint_scheduler.start();
                    }
                }
                else {
                    dynamic_config.set(VISITED_WAYPOINTS, visited_waypoints);

                    write_dynamic_config();
                    waypoint_scheduler.start();
                }
            }
        }
        // send update if there are new things to report
        double time_elapsed = current_time - last_update_time;
        if (got_fresh_info || time_elapsed >= UPDATE_INTERVAL_MS) {
            // TODO Maybe update ETA based on time elapsed since last update.
            // Not an immediately trivial change since ETAs might become negative.
            if (current_state.eta.has_value()) {
                TRACE(std::cerr << "Updating ETA: before: " << *current_state.eta);
                current_state.eta = std::max(0.0, current_state.eta.value() - time_elapsed / 1000);
                TRACE(std::cerr << "\tafter: " << *current_state.eta << std::endl);
            }
            send_robot_info();
            if (!(station_scheduler.running() || waypoint_scheduler.running() ||
                  eta_extractor.running())) {
                TRACE(std::cerr << "Orchestrator: writing dynamic config" << std::endl);
                write_dynamic_config();
            }
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
            auto dist = euclidean_distance(communication::Point{x, y}, controller_state.position);
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
        TRACE(std::cerr << "Orchestrator: Removing station " << station << " from remaining order"
                        << std::endl);
        order.erase(it);
        station_subscriber->pop();
    }
    visited_waypoints.push_back(station);
}

void robot::Orchestrator::clear_visited_waypoints()
{
    visited_waypoints.clear();
    dynamic_config.set(VISITED_WAYPOINTS, visited_waypoints);
}

void robot::Orchestrator::create_new_station_schedule()
{
    station_scheduler.maybe_start();
    station_scheduler.wait_for_result();
}

const std::filesystem::path wp_template_path =
    "../../wbt-translator/templates/waypoint_scheduling.q.template";
const std::filesystem::path eta_template_path = "../../wbt-translator/templates/get_eta.q.template";
const std::filesystem::path wp_output_path = "waypoint_scheduling.q";
const std::filesystem::path eta_output_path = "get_eta.q";

void robot::Orchestrator::create_query_file()
{
    instantiate_query_template(robot_info.size(), waypoints.size(), wp_template_path,
                               wp_output_path);
    instantiate_query_template(robot_info.size(), waypoints.size(), eta_template_path,
                               eta_output_path);
}

double robot::Orchestrator::dist_to_next_waypoint()
{
    auto &[x, _, y] = ast.nodes.at(next_waypoint.value).translation;
    return euclidean_distance(current_state.location, communication::Point{x, y}) /
           MEASURED_ROBOT_SPEED;
}
