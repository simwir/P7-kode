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

#ifndef ORDER_RANDOM_GENERATOR
#define ORDER_RANDOM_GENERATOR

#include "order/generator.hpp"
#include <ctime>
#include <vector>

namespace order {
class RandomGenerator : public Generator {
    const std::vector<int> stations;
    const int min_size;
    const int max_size;
    const int seed;

  public:
    RandomGenerator(std::vector<int> stations) : RandomGenerator(stations, time(0)){};
    RandomGenerator(std::vector<int> stations, unsigned seed)
        : RandomGenerator(stations, seed, 0){};
    RandomGenerator(std::vector<int> stations, unsigned seed, unsigned min_size)
        : RandomGenerator(stations, seed, min_size, stations.size()){};
    RandomGenerator(std::vector<int> stations, unsigned seed, unsigned min_size, unsigned max_size);
    int get_seed() const;
    int get_min_size() const;
    int get_max_size() const;
    Order generate_order() const;
    std::vector<Order> generate_n_orders(int n) const;
};
} // namespace order

#endif
