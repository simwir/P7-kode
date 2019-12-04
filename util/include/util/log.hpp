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
    NullLog &operator<<(const std::string &) { return *this; }
};

#endif
