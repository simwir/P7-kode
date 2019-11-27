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
#ifndef UPPAAL_EXECUTOR_HPP
#define UPPAAL_EXECUTOR_HPP

#include <filesystem>
#include <functional>
#include <iostream>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <atomic>

namespace scheduling {

struct AbortException : public std::exception {
    std::string message;
    AbortException(const std::string &msg) : message(msg) {}

    const char *what() const noexcept override { return message.c_str(); }
};
class SchedulingException : public std::exception {
    std::string message;

  public:
    SchedulingException(const std::string &inmessage) { message = inmessage; }

    const char *what() const noexcept override { return message.c_str(); }
};

class UppaalExecutor {
  public:
    UppaalExecutor(const char *modelPath, const char *queriesPath)
        : model_path(std::filesystem::path{modelPath}),
          query_path(std::filesystem::path{queriesPath})
    {
    }
    UppaalExecutor(const std::filesystem::path &model_path, const std::filesystem::path &query_path)
        : model_path(model_path), query_path(query_path)
    {
    }
    void execute(std::function<void(const std::string &)> callback);

    // return true if aborted successfully or there was nothing to abort.
    bool abort();

    bool joinable() const { return worker.joinable(); }

    void join() { worker.join(); }

    void wait_for_result()
    {
        if (joinable()) {
            join();
        }
    }

  private:
    const std::filesystem::path model_path;
    const std::filesystem::path query_path;

    void reset_pid(std::optional<int> val = std::nullopt)
    {
        std::scoped_lock _{pid_lock};
        child_pid = val;
    }
    bool has_pid()
    {
        std::scoped_lock _{pid_lock};
        return child_pid.has_value();
    }

    int get_pid()
    {
        std::scoped_lock _{pid_lock};
        return *child_pid;
    }

    std::thread worker;
    bool worker_active = false;
    std::mutex pid_lock;
    std::optional<int> child_pid;
};

struct Holds {
    bool &b;
    Holds(bool &b) : b(b) { b = true; }
    ~Holds() { b = false; }
};
} // namespace scheduling
#endif // UPPAAL_EXECUTOR_HPP
