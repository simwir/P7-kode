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

#include "order/generation_service.hpp"
#include "order/random_generator.hpp"
#include "util/split.hpp"
#include <getopt.h>
#include <iostream>
#include <vector>

using namespace order;

void print_help()
{
    std::cout
        << "--stations \"s1, s2, ..., sn\"  Sets the available stations\n"
           "--random <seed>                 Uses a random order generator with an optional seed\n"
           "--min <size>                    Minimum order size (used with random generator)\n"
           "--max <size>                    Maxmum order size (used with random generator)\n";
    exit(1);
}

std::vector<int> parse_stations_argument(const std::string &argument)
{
    std::vector<int> stations;

    for (std::string station : split(argument, ' ')) {
        try {
            stations.push_back(stoi(station));
        }
        catch (std::invalid_argument &) {
            std::cerr << "Station could not be converted to integer: " << station << std::endl;
        }
    }

    return stations;
}

int main(int argc, char *argv[])
{
    const option options[] = {{"stations", required_argument, 0, 's'},
                              {"random", optional_argument, 0, 'r'},
                              {"min", required_argument, 0, 'n'},
                              {"max", required_argument, 0, 'm'}};

    const std::string short_options{"s:n:m:r"};

    int argument_index = 0;
    int min_size;
    int max_size;
    unsigned seed;
    std::vector<int> stations;
    std::string generator_type;

    while (true) {
        int argument = getopt_long(argc, argv, short_options.c_str(), options, &argument_index);

        if (argument == -1) {
            break;
        }

        switch (argument) {
        case 'r':
            generator_type = "random";
            seed = atoi(optarg);
            break;
        case 's':
            stations = parse_stations_argument(optarg);
            break;
        case 'n':
            min_size = atoi(optarg);
            break;
        case 'm':
            max_size = atoi(optarg);
            break;
        default:
            print_help();
            break;
        }
    }

    std::shared_ptr<Generator> generator;

    if (generator_type == "random") {
        generator = std::make_shared<RandomGenerator>(stations, min_size, max_size, seed);
    }
    else {
        std::cerr << "Generator type not set";
        exit(1);
    }

    GenerationService service = GenerationService{5555, generator};
    service.start();

    return 0;
}
