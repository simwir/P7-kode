#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "util/json.hpp"
#include "util/meta.hpp"
#include <string>
#include <type_traits>
#include <vector>

namespace config {
struct InvalidValueException : std::exception {
    const char *what() const noexcept { return "Invalid value"; }
};

class InvalidKeyException : public std::exception {
    std::string message;

  public:
    InvalidKeyException(const std::string &key) : message("Key not found: " + key) { };
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
    void set(const std::string &key, T value)
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
