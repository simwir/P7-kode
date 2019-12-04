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

#include "order/random_generator.hpp"
#include "util/pick_random.hpp"

namespace order {
RandomGenerator::RandomGenerator(std::vector<int> stations, unsigned seed, unsigned min_size,
                                 unsigned max_size)
    : stations(stations), min_size(min_size), max_size(max_size), seed(seed)
{
    srand(seed);
};

int RandomGenerator::get_seed() const
{
    return seed;
}

int RandomGenerator::get_min_size() const
{
    return min_size;
}

int RandomGenerator::get_max_size() const
{
    return max_size;
}

Order RandomGenerator::generate_order() const
{
    int n = min_size + (rand() % (max_size - min_size + 1));
    return Order{pick_n_random(stations, n)};
}

std::vector<Order> RandomGenerator::generate_n_orders(int n) const
{
    std::vector<Order> orders;

    for (int i = 0; i < n; i++) {
        orders.push_back(generate_order());
    }

    return orders;
}
} // namespace order
