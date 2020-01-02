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
        if (!running()) {
            start();
        }
    }

    bool running() const { return executor.running(); }

    void abort()
    {
        if (!executor.abort())
            throw AbortException{"Could not abort"};
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
