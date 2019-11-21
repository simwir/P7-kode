#ifndef ORCHESTRATOR_CLOCK_HPP
#define ORCHESTRATOR_CLOCK_HPP

#include "tcp/client.hpp"
#include <string>

namespace robot {
class Clock {
  public:
    virtual int get_current_time() = 0;

    virtual ~Clock() = default;
};

class WebotsClock : public Clock {
  public:
    WebotsClock(const std::string &host, const std::string &port);

    int get_current_time() override;

  private:
    tcp::Client webots_client;
};

} // namespace robot
#endif
