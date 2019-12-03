#ifndef LOG_HPP
#define LOG_HPP

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <string>

class Log {

  public:
    Log(std::filesystem::path path) : log(path, std::ofstream::out | std::ofstream::app) {}
    Log() {}

    virtual Log &operator<<(const std::string &val)
    {
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        char *timestamp = std::ctime(&now);

        std::string timestampWithoutEndl{timestamp};
        timestampWithoutEndl = timestampWithoutEndl.substr(0, 24);

        log << "[" << timestampWithoutEndl << "]: " << val << std::endl;
        return *this;
    }

  private:
    std::ofstream log;
};

class NullLog {
public:
    NullLog &operator<<(const std::string &)
    {
        return *this;
    }
};

#endif
