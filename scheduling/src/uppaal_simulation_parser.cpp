#include <iostream>
#include <sstream>
#include <uppaal_simulation_parser.hpp>

std::vector<scheduling::SimulationExpression>
scheduling::UppaalSimulationParser::parse(const std::string& result, int formula)
{
    int startIndex = result.find("Verifying formula " + std::to_string(formula));
    int stopIndex =
        result.find("Verifying formula " +
                    std::to_string(formula + 1)); // Equal to std::string::npos if not found.

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

    while (line.size() > 0 && line.at(0) == '[') {
        // Find all (t, n) pairs
        int state = 0;
        std::stringstream time;
        std::stringstream value;
        std::stringstream run_number;
        std::vector<scheduling::TimeValuePair> values;

        // Regex for this (ignore whitespace) \[\d+\]: ( \( \d+(\.\d+)? , \d+ \) )*
        for (char &c : line) {
            if (isspace(c)) {
                continue;
            }

            switch (state) {
            case 0:
                if (c != '[') {
                    throw SimulationParseException("State 0 missing [");
                }
                state = 1;
                break;
            case 1:
                if (!isdigit(c)) {
                    throw SimulationParseException("State 1 missing digit");
                }
                run_number << c;
                state = 2;
                break;
            case 2:
                if (isdigit(c)) {
                    run_number << c;
                }
                else if (c == ']') {
                    state = 3;
                }
                else {
                    throw SimulationParseException("State 2 missing digit or ]");
                }
                break;
            case 3:
                if (c != ':') {
                    throw SimulationParseException("State 3 missing :");
                }
                state = 4;
                break;
            case 4:
                if (c != '(') {
                    throw SimulationParseException("State 4 missing (");
                }
                state = 5;
                break;
            case 5:
                if (!isdigit(c)) {
                    throw SimulationParseException("State 5 missing digit");
                }
                time << c;
                state = 6;
                break;
            case 6:
                if (isdigit(c)) {
                    time << c;
                }
                else if (c == '.') {
                    time << c;
                    state = 7;
                }
                else if (c == ',') {
                    state = 9;
                }
                else {
                    throw SimulationParseException("State 6 missing digit, . or ,");
                }
                break;
            case 7:
                if (!isdigit(c)) {
                    throw SimulationParseException("State 7 missing digit");
                }
                time << c;
                state = 8;
                break;
            case 8:
                if (isdigit(c)) {
                    time << c;
                }
                else if (c == ',') {
                    state = 9;
                }
                else {
                    throw SimulationParseException("State 8 missing digit or ,");
                }
                break;
            case 9:
                if (!isdigit(c)) {
                    throw SimulationParseException("State 9 missing digit");
                }
                value << c;
                state = 10;
                break;
            case 10:
                if (isdigit(c)) {
                    value << c;
                }
                else if (c == ')') {
                    double time_double = std::stod(time.str());
                    int value_int = std::stoi(value.str());
                    // Clear string streams
                    time.str(std::string());
                    value.str(std::string());

                    values.push_back(scheduling::TimeValuePair{time_double, value_int});

                    state = 4;
                }
                else {
                    throw SimulationParseException("State 10 missing digit or )");
                }
                break;
            }
        }

        runs.push_back(scheduling::SimulationTrace{std::stoi(run_number.str()), values});

        if (ss.eof()) {
            break;
        }

        // Read next run
        std::getline(ss, line);
    }

    return scheduling::SimulationExpression{name, runs};
}
