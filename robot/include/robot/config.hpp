#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <json/json.h>

#include <string>
#include <type_traits>
#include <vector>

namespace meta {

template <typename T, typename = void>
struct is_container : std::false_type {
};

template <typename T>
using element_type = decltype(std::begin(std::declval<T &>()));

template <typename T>
struct is_container<T, std::void_t<element_type<T>>> : std::true_type {
};
template <typename T>
constexpr auto is_container_v = is_container<T>::value;

template <typename T, typename = void>
struct is_string : std::false_type {
};

template <typename T>
struct is_string<T, std::enable_if_t<is_container<T>::value>>
    : std::is_same<element_type<T>, char> {
};

} // namespace meta

namespace robot {
struct InvalidValueException : std::exception {
    const char *what() const noexcept { return "Invalid value"; }
};

class InvalidKeyException : public std::exception {
    std::string message;

  public:
    InvalidKeyException(const std::string &key) { message = "Key not found: " + key; };
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
        if constexpr (std::is_assignable<Json::Value&, T>::value) {
            json[key] = value;
        }
        else {
            static_assert(meta::is_container<T>::value, "expected container");
            Json::Value arr{Json::arrayValue};
            for (auto &it : value) {
                arr.append(it);
            }
            json[key] = arr;
        }
    }
};
} // namespace robot

#endif
