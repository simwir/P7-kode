#ifndef LOG_HPP
#define LOG_HPP

#include <fstream>
#include <filesystem>
#include <string>
#include <chrono>
#include <ctime>

class Log {

public:
  Log(std::filesystem::path path) : log(path, std::ofstream::out | std::ofstream::app) { }

  void operator<<(const std::string& val) {
      std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
      char* timestamp = std::ctime(&now);

      std::string timestampWithoutEndl(timestamp);
      timestampWithoutEndl = timestampWithoutEndl.substr(0, 24);

      log << "["<< timestampWithoutEndl << "]: " << val << std::endl;
  }

private:
  std::ofstream log;
};

#endif
