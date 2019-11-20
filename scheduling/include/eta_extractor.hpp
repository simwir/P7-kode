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
#ifndef SCHEDULING_ETA_EXTRACTOR
#define SCHEDULING_ETA_EXTRACTOR

#include "uppaal_executor.hpp"

#include <filesystem>
#include <memory>
#include <thread>
#include <vector>

namespace scheduling {

struct StrategyNotDefinedException : std::exception {
    const std::string _what;
    template <class string_type>
    StrategyNotDefinedException(string_type &&msg) : _what(msg)
    {
    }
    const char *what() const noexcept override { return _what.c_str(); }
};

class EtaSubscriber : public std::enable_shared_from_this<EtaSubscriber> {
  public:
    virtual void new_eta(double eta) = 0;
    virtual ~EtaSubscriber() = default;
};

class EtaExtractor {
  public:
    EtaExtractor() : executor(default_model_path, default_query_path) {}
    bool eta_computable() const { return std::filesystem::exists(strategy_path); }
    void start();
    void wait_for_eta();
    void addSubscriber(std::shared_ptr<EtaSubscriber> subscriber)
    {
        subscribers.push_back(subscriber->weak_from_this());
    }

  private:
    UppaalExecutor executor;
    std::vector<std::weak_ptr<EtaSubscriber>> subscribers;
    void notify_eta(double eta);

    std::thread runner;
    void run();

    // TODO WIP while PR #22 is not yet merged.
    constexpr static auto *default_model_path = "waypoint_scheduling.xml";
    constexpr static auto *default_query_path = "get_eta.q";
    constexpr static auto *default_strat_path = "waypoint_strategy.json";
    const std::filesystem::path model_file = std::filesystem::path{default_model_path};
    const std::filesystem::path query_file = std::filesystem::path{default_query_path};
    const std::filesystem::path strategy_path = std::filesystem::path{default_strat_path};
};

} // namespace scheduling

#endif
