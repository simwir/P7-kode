#ifndef UPPAAL_EXECUTOR_HPP
#define UPPAAL_EXECUTOR_HPP

#include <iostream>
#include <string>
#include <optional>

namespace scheduling {

class SchedulingException : public std::exception {
    std::string message;

  public:
    SchedulingException(const std::string &inmessage) { message = inmessage; }

    const char *what() const noexcept override { return message.c_str(); }
};

class UppaalExecutor {
  public:
    UppaalExecutor(const char *modelPath, const char *queriesPath)
        : modelPath(modelPath), queriesPath(queriesPath)
    {
    }
    std::optional<std::string> execute();

    bool abort();

  private:
    const char *modelPath;
    const char *queriesPath;

    std::optional<int> child_pid;
};

} // namespace scheduling
#endif // UPPAAL_EXECUTOR_HPP
