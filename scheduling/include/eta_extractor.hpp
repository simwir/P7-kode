#ifndef SCHEDULING_ETA_EXTRACTOR
#define SCHEDULING_ETA_EXTRACTOR

#include "uppaal_executor.hpp"

#include <memory>
#include <thread>
#include <vector>
#include <filesystem>

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
