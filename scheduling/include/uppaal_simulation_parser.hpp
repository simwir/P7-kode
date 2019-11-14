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
#ifndef UPPAAL_SIMULATION_PARSER_H
#define UPPAAL_SIMULATION_PARSER_H

#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>

namespace scheduling {

struct FormulaNotSatisfiedException : std::exception {
    const char *what() const noexcept override { return "Formula not satisfied"; }
};

class SimulationParseException : public std::exception {
    std::string message;

  public:
    SimulationParseException(const std::string &inmessage) : message(inmessage) {}

    const char *what() const noexcept override { return message.c_str(); }
};

struct NameNotFoundException : std::exception {
    const char *what() const noexcept override { return "Cannot find name"; }
};

struct TimeValuePair {
    double time;
    int value;
};

struct SimulationTrace {
    int number;
    std::vector<TimeValuePair> values;
};

struct SimulationExpression {
    std::string name;
    std::vector<SimulationTrace> runs;
};

class UppaalSimulationParser {
  public:
    std::vector<SimulationExpression> parse(const std::string &result, int formula_number);
    std::queue<TimeValuePair> findFirstRunAsQueue(const std::vector<SimulationExpression> &values,
                                                  const std::string &name);

  private:
    SimulationExpression parseValue(std::istream &ss, std::string &line);
};

} // namespace scheduling

#endif // UPPAAL_SIMULATION_PARSER_H
