#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <memory>
#include <sstream>
#include <thread>

using logger_ptr = std::shared_ptr<spdlog::logger>;

class Logger {
  public:
    Logger() : tid(std::this_thread::get_id()), _logger(make_logger()) {}

    template <typename... Ts>
    void info(Ts &&... args)
    {
        _logger->info(std::forward<Ts>(args)...);
    }

    template <typename... Ts>
    void warn(Ts &&... args)
    {
        _logger->warn(std::forward<Ts>(args)...);
    }

    template <typename... Ts>
    void error(Ts &&... args)
    {
        _logger->error(std::forward<Ts>(args)...);
    }

    const std::thread::id tid;

  private:
    static logger_ptr make_logger()
    {
        std::stringstream s;
        s << std::this_thread::get_id();
        auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        logger_ptr logger = std::make_shared<spdlog::logger>(s.str(), sink);
        logger->set_pattern("[%H:%M:%S %z] [thread %t] [%^%l%$] %v");
        return logger;
    }
    logger_ptr _logger;
};

extern thread_local Logger logger;

#endif
