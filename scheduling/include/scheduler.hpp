#ifndef SCHEDULING_SCHEDULER_HPP
#define SCHEDULING_SCHEDULER_HPP

#include "uppaal_executor.hpp"

#include <filesystem>
#include <memory>
#include <thread>
#include <vector>

namespace scheduling {

template <class Subscriber, class Notification>
class Scheduler {
    static_assert(std::is_base_of<std::enable_shared_from_this<Subscriber>, Subscriber>::value,
                  "subscriber type must enable shared from this.");

  public:
    Scheduler(const char *model_path, const char *query_path) : executor(model_path, query_path) {}
    Scheduler(const std::filesystem::path &model_path, const std::filesystem::path &query_path)
        : executor(model_path, query_path)
    {
    }

    void start()
    {
        abort();
        start_worker();
    }

    void maybe_start()
    {
        if (!executor.running()) {
            start();
        }
    }

    void abort()
    {
        if (!executor.abort())
            throw AbortException{""};
    }

    void wait_for_result() { executor.wait_for_result(); }

    void add_subscriber(std::shared_ptr<Subscriber> subscriber)
    {
        subscribers.push_back(subscriber->weak_from_this());
    }

  protected:
    virtual void start_worker() = 0;
    std::vector<std::weak_ptr<Subscriber>> subscribers;
    UppaalExecutor executor;
    virtual void notify_subscribers(const Notification &) = 0;
    virtual ~Scheduler() = default;
};

} // namespace scheduling

#endif
