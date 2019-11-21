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
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "util/json.hpp"
#include "util/meta.hpp"
#include <string>
#include <type_traits>
#include <vector>

namespace config {
class InvalidValueException : public std::exception {
    std::string message;

  public:
    InvalidValueException(const std::string &str) : message("Invalid value: " + str){};
    const char *what() const noexcept { return message.c_str(); }
};

class InvalidKeyException : public std::exception {
    std::string message;

  public:
    InvalidKeyException(const std::string &key) : message("Key not found: " + key){};
    const char *what() const noexcept { return message.c_str(); };
};

template <typename T>
T convert_from_json(const Json::Value &value);

class Config {
    Json::Value json;

  public:
    Config(){};
    Config(const std::string &file_path);
    void load_from_file(const std::string &file_path);
    void write_to_file(const std::string &file_path);

    template <typename T>
    T get(const std::string &key);

    template <typename T>
    T get(const std::string &key1, const std::string &key2);

    int getSize(const std::string &key);

    template <typename T>
    void set(const std::string &key, T &&value)
    {
        // if we can just assign the value to json[key] (of type Json::Value &),
        // simply assign.
        if constexpr (std::is_assignable<Json::Value &, T>::value) {
            json[key] = value;
        }
        // if it is a container, dump container to a Json::array.
        else if constexpr (meta::is_container<T>::value) {
            Json::Value arr{Json::arrayValue};
            for (auto &it : value) {
                arr.append(it);
            }
            json[key] = arr;
        }
        // otherwise abort; we don't know what to do;
        else {
            meta::unsupported<T> _;
        }
    }
};
} // namespace config

#endif
