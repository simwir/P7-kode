#include "robot/clock.hpp"

namespace robot {

WebotsClock::WebotsClock(const std::string &host, const std::string &port)
    : webots_client(host, port)
{
}

int WebotsClock::get_current_time()
{
    auto msg = webots_client.atomic_blocking_request("get_time");
    try {
        return std::stoi(msg);
    }
    catch (std::invalid_argument &e) {
        throw std::logic_error{"Error: couldn't parse time from " + msg};
    }
}
} // namespace robot
