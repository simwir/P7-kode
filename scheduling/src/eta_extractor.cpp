#include "eta_extractor.hpp"

#include <algorithm>
#include <chrono>
#include <regex>
#include <thread>

namespace scheduling {
void EtaExtractor::start()
{
    running = true;
    runner = std::thread(&EtaExtractor::run, this);
}

void EtaExtractor::stop()
{
    running = false;
    runner.join();
}

void EtaExtractor::run()
{
    while (running) {
        if (!eta_computable()) {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1s);
            continue;
        }
        auto res = executor.execute();
        std::regex eta_response{R"(.+= ([\d\.]+))"};
        std::smatch eta_value;
        if (std::regex_search(res, eta_value, eta_response)) {
            // index 0 is the whole string
            notify_eta(stod(eta_value[1]));
        }
    }
}

void EtaExtractor::notify_eta(double eta)
{
    std::for_each(std::begin(subscribers), std::end(subscribers), [eta](auto &&subscriber) {
        if (auto sub = subscriber.lock())
            sub->new_eta(eta);
    });
}
} // namespace scheduling
