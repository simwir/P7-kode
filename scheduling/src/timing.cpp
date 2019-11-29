#include <filesystem>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <string>
#include <regex>
#include <random>

#include "config/config.hpp"
#include "wbt-translator/apsp.hpp"
#include "wbt-translator/distance_matrix.hpp"
#include "wbt-translator/query_template_writer.hpp"
#include "wbt-translator/webots_parser.hpp"
#include "uppaal_executor.hpp"
#include "communication/info.hpp"

config::Config static_config;
config::Config dynamic_config;

std::vector<int> stations;
std::vector<int> end_stations;
std::vector<int> vias;
std::vector<int> waypoints;

constexpr int iterations = 100;
constexpr int max_robots = 20;

const std::filesystem::path dynamic_conf = "dynamic_config.json";
const std::filesystem::path static_conf = "static_config.json";
const std::filesystem::path world_path = "testmap_1.wbt";

#define NEXT_WAYPOINT "next_waypoint"
#define NEXT_STATION "next_station"
#define ROBOT_INFO_MAP "robot_info_map"
#define SELF_STATE "self_state"
#define STATION_ETA "station_eta"
#define VISITED_STATIONS "visited_stations"
#define VISITED_WAYPOINTS "visited_waypoints"
#define STATIONS_TO_VISIT "stations_to_visit"

#define STATIONS "stations"
#define END_STATIONS "end_stations"
#define VIAS "vias"

// constant parameters for the UPPAAL model.
constexpr int STATION_DELAY = 6;    // time the robots remain at each station
constexpr int WAYPOINT_DELAY = 3;   // time the robots remain at each waypoint
constexpr double UNCERTAINTY = 1.1; // statistical uncertainty on all times.

std::default_random_engine generator;

AST get_ast() {
    std::ifstream world_file{world_path};
    Parser webots_parser{world_file};
    return webots_parser.parse_stream();
}

void add_waypoint_matrix(const AST &ast)
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

void add_station_matrix(const AST &ast)
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

void load_webots_to_config(const AST& ast)
{
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

    add_waypoint_matrix(ast);
    add_station_matrix(ast);
    static_config.set("station_delay", STATION_DELAY);
    static_config.set("waypoint_delay", WAYPOINT_DELAY);
    static_config.set("uncertainty", UNCERTAINTY);
}

std::string extractFormula(const std::string& result, int formula_number) {
    const size_t startIndex = result.find("Verifying formula " + std::to_string(formula_number));
    const size_t stopIndex =
        result.find("Verifying formula " +
                    std::to_string(formula_number + 1)); // Equal to std::string::npos if not found.

    if (stopIndex == std::string::npos) {
        return result.substr(startIndex);
    }
    else {
        return result.substr(startIndex, stopIndex - startIndex);
    }
}

void station_iteration(int number_of_robots, std::ofstream& output, scheduling::UppaalExecutor& executor) {
    std::uniform_real_distribution<double> eta_distribution(0, 20);

    // Next station
    std::uniform_int_distribution<int> station_distribution(0, stations.size() - 1);
    int next_station = station_distribution(generator);

    // Robot info map
    std::vector<robot::Info> infos;
    for (int i = 0; i < number_of_robots; i++) {
        std::vector<int> station_copy = stations;
        std::random_shuffle(station_copy.begin(), station_copy.end());
        station_copy.resize(station_distribution(generator));
        double eta = eta_distribution(generator);
        robot::Info info{i, {0,0},
            station_copy,
            std::vector<scheduling::Action>{},
            std::optional<double>{eta}};

        infos.push_back(info);
    }

    // Station_eta
    double eta = eta_distribution(generator);

    // Stations to visit
    std::vector<int> station_copy = stations;
    std::remove(station_copy.begin(), station_copy.end(), next_station);
    std::random_shuffle(station_copy.begin(), station_copy.end());

    std::poisson_distribution<int> remaining_distribution(station_copy.size() * 0.2);
    int new_size = station_copy.size() - 1 - remaining_distribution(generator);
    int recified_new_size = std::max(new_size, 1);
    station_copy.resize(recified_new_size);

    // Write dynamic
    dynamic_config.set(ROBOT_INFO_MAP, robot::InfoMap{infos}.to_json());
    dynamic_config.set(STATION_ETA, eta);
    dynamic_config.set(NEXT_STATION, next_station);
    dynamic_config.set(STATIONS_TO_VISIT, station_copy);
    dynamic_config.write_to_file(dynamic_conf);

    // Run
    double naive_expectation = 0;
    double smart_expectation = 0;
    auto callback = [&naive_expectation, &smart_expectation](const std::string &result) {
        std::regex eta_response{R"(.+= ([\d\.]+))"};
        std::smatch eta_value;

        std::string naive_formula = extractFormula(result, 2);
        if (std::regex_search(naive_formula, eta_value, eta_response)) {
            naive_expectation = std::stod(eta_value[1]);
        }

        std::string smart_formula = extractFormula(result, 3);
        if (std::regex_search(smart_formula, eta_value, eta_response)) {
            smart_expectation = std::stod(eta_value[1]);
        }
    };

    auto start = std::chrono::high_resolution_clock::now();
    executor.execute(callback);
    executor.wait_for_result();
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;

    output << number_of_robots << "," << recified_new_size << "," << elapsed.count() << "," << naive_expectation << "," << smart_expectation << std::endl;
}

void test_station_scheduling() {
    std::ofstream result{world_path.string() + "_station_result.csv"};
    result << "number_of_robots,number_to_visit,scheduling_time_seconds,naive_expectation,smart_expectation" << std::endl;

    scheduling::UppaalExecutor executor{"station_scheduling.xml", "station_timing.q"};

    for (int number_of_robots = 0; number_of_robots <= max_robots; number_of_robots += 20) {
        for (int i = 0; i < iterations; i++) {
            std::cout << "Running station timing (" << world_path << ", " << number_of_robots << ", " << i << ")" << std::endl;
            station_iteration(number_of_robots, result, executor);
        }
    }
}

void waypoint_iteration(int number_of_robots, std::ofstream& output) {
    // Next station
    std::uniform_int_distribution<int> station_distribution(0, stations.size() - 1);
    int next_station = station_distribution(generator);

    // Next waypoint
    std::uniform_int_distribution<int> via_distribution(0, vias.size() - 1);
    int next_waypoint = via_distribution(generator);

    // Robot info map

    // visited waypoints

    // Generate query file

    // Run

    // Extract expectation for naive and smart strategy
}

void test_waypoint_scheduling() {
    std::ofstream result{world_path.string() + "_waypoint_result.csv"};
    result << "number_of_robots,scheduling_time_seconds,naive_expectation,smart_expectation" << std::endl;

    for (int number_of_robots = 0; number_of_robots <= max_robots; number_of_robots += 20) {
        for (int i = 0; i < iterations; i++) {
            std::cout << "Running waypoint timing (" << world_path << ", " << number_of_robots << ", " << i << ")" << std::endl;
            waypoint_iteration(number_of_robots, result);
        }
    }
}

int main(int argc, char **argv) {
    std::srand(unsigned(std::time(0)));

    AST ast = get_ast();
    load_webots_to_config(ast);

    static_config.write_to_file(static_conf);

    if (std::strcmp(argv[1], "station") == 0) {
        test_station_scheduling();
    }
    else if (std::strcmp(argv[1], "waypoint") == 0) {
        test_waypoint_scheduling();
    }
}
