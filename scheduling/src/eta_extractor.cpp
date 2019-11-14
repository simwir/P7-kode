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
    executor.execute([&](const std::string& res) {
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
