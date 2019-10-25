#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <json/json.h>

#include <string>

namespace robot {
class InvalidKeyException : public std::exception {
    std::string message;

  public:
    InvalidKeyException(const std::string &key) { message = "Key not found: " + key; };
    const char *what() const noexcept { return message.c_str(); };
};

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
        json[key] = value;
    };
};
} // namespace robot

#endif
