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
#ifndef WEBOTS_PARSER_H
#define WEBOTS_PARSER_H

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class EndOfStreamException : public std::exception {
    std::string message;

  public:
    EndOfStreamException(const std::string &inmessage) { message = inmessage; }

    const char *what() const noexcept override { return message.c_str(); }
};
class MalformedWorldFileError : public std::exception {
    std::string message;

  public:
    MalformedWorldFileError(const std::string &inmessage)
    {
        message = "Malformed world file:" + inmessage;
    }

    const char *what() const noexcept override { return message.c_str(); }
};

struct Translation {
    double x, y, z;
};

enum class WaypointType { eVia, eStation, eEndPoint };

std::string to_string(WaypointType wptype);

struct Waypoint {
    int id;
    std::vector<int> adjlist;
    Translation translation;
    WaypointType waypointType;
};

struct AST {
    std::map<size_t, Waypoint> nodes;

    bool are_connected(size_t i, size_t j) const
    {
        auto start = nodes.find(i);
        if (start != std::end(nodes)) {
            std::vector<int> adjlist = start->second.adjlist;
            // Does the adjlist contain the element j?
            return std::find(std::begin(adjlist), std::end(adjlist), j) != std::end(adjlist);
        }
        else {
            throw MalformedWorldFileError("The waypoint " + std::to_string(i) +
                                          " Could not be found");
        }
    }

    size_t num_waypoints() const {
        return nodes.size();
    }
};

class Parser {
  public:
    AST parse_stream();
    Parser(std::istream &inputStream) : is(inputStream) {}
    int number_of_robots;

  private:
    std::istream &is;
    AST ast;

    std::string read_token();
    std::string read_string();
    Waypoint parse_waypoint(const std::string &);
    Translation parse_translation();
    int parse_id();
    std::vector<int> parse_adjlist();
};

#endif
