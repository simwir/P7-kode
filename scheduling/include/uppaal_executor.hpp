#ifndef UPPAAL_EXECUTOR_HPP
#define UPPAAL_EXECUTOR_HPP

#include <iostream>
#include <string>
#include <filesystem>

namespace scheduling {

class SchedulingException : public std::exception {
    std::string message;

  public:
    SchedulingException(const std::string &inmessage) : message(inmessage) {}

    const char *what() const noexcept override { return message.c_str(); }
};

class UppaalExecutor {
  public:
    UppaalExecutor(const char* modelPath, const char* queriesPath)
        : model_path(std::filesystem::path{modelPath}), query_path(std::filesystem::path{queriesPath})
    {
    }
    UppaalExecutor(const std::filesystem::path &model_path, const std::filesystem::path &query_path)
        :model_path(model_path), query_path(query_path) {}
    std::string execute();

  private:
    const std::filesystem::path model_path;
    const std::filesystem::path query_path;
};

} // namespace scheduling

#endif // UPPAAL_EXECUTOR_HPP
