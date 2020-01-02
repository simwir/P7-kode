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
#include <algorithm>
#include <iostream>
#include <regex>
#include <sstream>
#include <uppaal_simulation_parser.hpp>

std::queue<scheduling::TimeValuePair> scheduling::UppaalSimulationParser::findFirstRunAsQueue(
    const std::vector<scheduling::SimulationExpression> &values, const std::string &name)
{
    auto iter = std::find_if(values.begin(), values.end(),
                             [&name](const scheduling::SimulationExpression &val) {
                                 return val.name.compare(name) == 0;
                             });

    if (iter == values.end()) {
        throw NameNotFoundException();
    }

    scheduling::SimulationTrace first_run = iter->runs.at(0);
    return std::queue<scheduling::TimeValuePair>(
        std::deque<scheduling::TimeValuePair>(first_run.values.begin(), first_run.values.end()));
}

std::vector<scheduling::SimulationExpression>
scheduling::UppaalSimulationParser::parse(const std::string &result, int formula_number)
{
    const size_t startIndex = result.find("Verifying formula " + std::to_string(formula_number));
    const size_t stopIndex =
        result.find("Verifying formula " +
                    std::to_string(formula_number + 1)); // Equal to std::string::npos if not found.

    if (startIndex == std::string::npos) {
        throw SimulationParseException{"Could not find formula " + std::to_string(formula_number) +
                                       " in output " + result};
    }

    std::string formula;
    if (stopIndex == std::string::npos) {
        formula = result.substr(startIndex);
    }
    else {
        formula = result.substr(startIndex, stopIndex - startIndex);
    }

    std::stringstream ss{formula};
    std::vector<scheduling::SimulationExpression> values;

    std::string line;
    std::getline(ss, line); // Verifying formula \d+ at <file:line>
    std::getline(ss, line); // -- Formula is (not)? satisfied.

    if (ss.eof()) {
        return values;
    }

    std::getline(ss, line);
    while (line.size() > 0) {
        values.push_back(parseValue(ss, line));

        if (ss.eof()) {
            break;
        }
    }

    return values;
}

scheduling::SimulationExpression scheduling::UppaalSimulationParser::parseValue(std::istream &ss,
                                                                                std::string &line)
{
    std::string name = line.substr(0, line.length() - 1);
    std::vector<scheduling::SimulationTrace> runs;

    // Read run
    std::getline(ss, line);

    std::string time;
    std::string value;

    // Matches full row of simulation results on the form: [run_number]: (time,value)
    // (time,value)... group 1 == run_number, group 2 == all time-value pairs.
    static const std::regex line_pattern{R"(\[(\d+)\]:((?: \(-?\d+(?:\.\d+)?,-?\d+\))*))"};
    // matches single (time,value) pair.
    // group 1 == time, group 2 == value.
    static const std::regex pair_pattern{R"( \((-?\d+(?:\.\d+)?),(-?\d+)\))"};

    std::smatch line_match;
    std::smatch pair_match;
    while (std::regex_match(line, line_match, line_pattern)) {
        std::vector<scheduling::TimeValuePair> values;
        int run_number = std::stoi(line_match[1]);
        std::string pairs = line_match[2];

        // parse the list of value pairs
        while (std::regex_search(pairs, pair_match, pair_pattern)) {
            double time = std::stod(pair_match[1]);
            int value = std::stoi(pair_match[2]);
            values.push_back(TimeValuePair{time, value});
            pairs = pair_match.suffix();
        }

        runs.push_back(SimulationTrace{run_number, values});
        if (ss.eof()) {
            break;
        }

        std::getline(ss, line);
    }

    return scheduling::SimulationExpression{name, runs};
}
