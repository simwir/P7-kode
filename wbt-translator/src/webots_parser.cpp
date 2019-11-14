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
#include "wbt-translator/webots_parser.hpp"
#include <iomanip>

std::string Parser::read_token()
{
    std::string token;
    if (!(is >> token)) {
        if (is.eof()) {
            throw EndOfStreamException("The stream ended while trying to read a token.");
        }
    }
    return token;
}

std::string Parser::read_string()
{
    std::string line;
    is >> std::quoted(line);
    return line;
}

AST Parser::parse_stream()
{
    number_of_robots = 0;
    try {
        while (!is.eof()) {
            std::string token = read_token();
            if (token == "Waypoint" || token == "Station" || token == "Endpoint") {
                Waypoint waypoint = parse_waypoint(token);
                ast.nodes.insert(std::make_pair(waypoint.id, waypoint));
            }
            else if (token == "Elisa3") {
                number_of_robots++;
            }
        };
    }
    catch (EndOfStreamException &e) {
    }
    return ast;
}

Waypoint Parser::parse_waypoint(const std::string &waypointType)
{
    Waypoint waypoint;
    if (waypointType == "Waypoint") {
        waypoint.waypointType = WaypointType::eVia;
    }
    else if (waypointType == "Station") {
        waypoint.waypointType = WaypointType::eStation;
    }
    else if (waypointType == "Endpoint") {
        waypoint.waypointType = WaypointType::eEndPoint;
    }
    else {
        throw MalformedWorldFileError("Unexpected waypoint type.");
    }
    bool needId = true, needAdj = true, needTranslation = true;
    try {
        while (needId || needAdj || needTranslation) {
            std::string token = read_token();
            if (token == "translation") {
                waypoint.translation = parse_translation();
                needTranslation = false;
            }
            else if (token == "id") {
                waypoint.id = parse_id();
                needId = false;
            }
            else if (token == "adjList") {
                waypoint.adjlist = parse_adjlist();
                needAdj = false;
            }
        }
    }
    catch (EndOfStreamException &e) {
        throw MalformedWorldFileError("Unexpected End of stream. World file malformed?");
    }
    return waypoint;
}

Translation Parser::parse_translation()
{
    Translation translation;
    translation.x = std::stod(read_token());
    translation.y = std::stod(read_token());
    translation.z = std::stod(read_token());
    return translation;
}

int Parser::parse_id()
{
    return std::stoi(read_token());
}

std::vector<int> Parser::parse_adjlist()
{
    std::string adjString = read_string();
    size_t pos = 0;
    std::vector<int> adjlist;
    try {
        while ((pos = adjString.find(",")) != std::string::npos) {
            adjlist.push_back(std::stoi(adjString.substr(0, pos)));
            adjString.erase(0, pos + 1);
        }
        adjlist.push_back(std::stoi(adjString));
    }
    catch (std::invalid_argument &e) {
        throw MalformedWorldFileError("Adjlist is not a list of integers. At: " + adjString);
    }
    return adjlist;
}
