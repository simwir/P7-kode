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
#include <string>
#include <vector>

template <typename T>
T pick_random(const std::vector<T> &choices)
{
    return choices[rand() % choices.size()];
}

template <typename T>
std::vector<T> pick_n_random(std::vector<T> choices, int n)
{
    if (n > choices.size()) {
        throw std::invalid_argument{"Cannot pick " + std::to_string(n) +
                                    " elements from a vector of size " +
                                    std::to_string(choices.size())};
    }

    std::vector<T> selected;

    // Pick a random element, add to selected and remove from choices. Ensures
    // no duplicates in the selected vector.
    for (int i = 0; i < n; ++i) {
        int element = pick_random(choices);
        selected.push_back(element);
        choices.erase(std::remove(choices.begin(), choices.end(), element), choices.end());
    }

    return selected;
}
