#ifndef UPPAAL_EXECUTOR_HPP
#define UPPAAL_EXECUTOR_HPP

#include <functional>
#include <thread>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

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
    void execute(std::function<void(const std::string&)>);

    // return true if aborted successfully or there was nothing to abort.
    bool abort();

    bool joinable() const {
        return worker.joinable();
    }

    void join() {
        worker.join();
    }

    void wait_for_result() {
        if (joinable()) {
            join();
        }
    }

  private:
    const std::filesystem::path model_path;
    const std::filesystem::path query_path;

    std::thread worker;
    std::optional<int> child_pid;
};

} // namespace scheduling
#endif // UPPAAL_EXECUTOR_HPP
