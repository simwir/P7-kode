#ifndef UPPAAL_SIMULATION_PARSER_H
#define UPPAAL_SIMULATION_PARSER_H

#include <string>
#include <vector>
#include <utility>

namespace scheduling {

struct FormulaNotSatisfiedException : public std::exception {
    const char* what() const noexcept override{
        return "Formula not satisfied";
    }
};

class SimulationParseException : public std::exception {
    std::string message;
public:
    SimulationParseException(const std::string& inmessage){
        message = inmessage;
    }

    const char* what() const noexcept override{
        return message.c_str();
    }
};

struct Run {
    int number;
    std::vector<std::pair<double, int>> values;
    
    Run(int number, std::vector<std::pair<double, int>> values) : number(number), values(values) { }
};

struct SimulationValue {
    std::string name;
    std::vector<Run> runs;
    
    SimulationValue(std::string name, std::vector<Run> runs) : name(name), runs(runs) { }
};

class UppaalSimulationParser {
public:
    std::vector<SimulationValue> parse(std::string result, int formula);
private:
    SimulationValue parseValue(std::stringstream& ss, std::string& line);
};
    
}

#endif // UPPAAL_SIMULATION_PARSER_H