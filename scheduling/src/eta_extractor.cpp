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
#include "eta_extractor.hpp"

#include <algorithm>
#include <chrono>
#include <regex>
#include <thread>

namespace scheduling {
void EtaExtractor::start_worker()
{
    if (!eta_computable()) {
        std::cerr << "WARNING: could not start eta extractor due to missing strategy file"
                  << std::endl;
        return;
    }
    executor.execute([&](const std::string &res) {
        std::regex eta_response{R"(.+= ([\d\.]+))"};
        std::smatch eta_value;
        if (std::regex_search(res, eta_value, eta_response)) {
            // index 0 is the whole string
            notify_subscribers(stod(eta_value[1]));
        }
    });
}

void EtaExtractor::notify_subscribers(const double &eta)
{
    std::for_each(std::begin(subscribers), std::end(subscribers), [eta](auto &&subscriber) {
        if (auto sub = subscriber.lock())
            sub->new_eta(eta);
    });
}
} // namespace scheduling
