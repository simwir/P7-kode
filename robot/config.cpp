#include "config.hpp"

#include <fstream>
#include <map>
#include <string>
#include <vector>

template <>
std::vector<int> robot::convert_from_json<std::vector<int>>(const Json::Value &value)
{
    if (value.type() != Json::ValueType::arrayValue) {
        throw robot::InvalidValueException();
    }

    std::vector<int> result;

    for (auto itr = value.begin(); itr != value.end(); itr++) {
        result.push_back(value[itr.index()].asInt());
    }

    return result;
}

robot::Config::Config(const std::string &file_path)
{
    load_from_file(file_path);
}

void robot::Config::load_from_file(const std::string &file_path)
{
    std::ifstream config_file(file_path);
    config_file >> json;
}

void robot::Config::write_to_file(const std::string &file_path)
{
    std::ofstream config_file(file_path);
    config_file << json;
}

template <>
int robot::Config::get<int>(const std::string &key)
{
    if (!json.isMember(key)) {
        throw robot::InvalidKeyException(key);
    };

    return json[key].asInt();
}

template <>
std::string robot::Config::get<std::string>(const std::string &key)
{
    if (!json.isMember(key)) {
        throw robot::InvalidKeyException(key);
    };

    return json[key].asString();
}

template <>
double robot::Config::get<double>(const std::string &key)
{
    if (!json.isMember(key)) {
        throw robot::InvalidKeyException(key);
    };

    return json[key].asDouble();
}

template <>
std::vector<int> robot::Config::get<std::vector<int>>(const std::string &key)
{
    if (!json.isMember(key)) {
        throw robot::InvalidKeyException(key);
    }

    if (json[key].type() != Json::ValueType::arrayValue) {
        throw robot::InvalidValueException();
    }

    return robot::convert_from_json<std::vector<int>>(json[key]);
}

template <>
std::map<int, std::vector<int>>
robot::Config::get<std::map<int, std::vector<int>>>(const std::string &key)
{
    if (!json.isMember(key)) {
        throw robot::InvalidKeyException(key);
    }

    if (json[key].type() != Json::ValueType::objectValue) {
        throw robot::InvalidValueException();
    }

    std::map<int, std::vector<int>> result;

    for (auto itr = json[key].begin(); itr != json[key].end(); itr++) {
        std::vector<int> value = robot::convert_from_json<std::vector<int>>(json[key][itr.name()]);
        result.insert({itr.key().asInt(), value});
    }

    return result;
}

template <typename T>
void robot::Config::set(const std::string &key, T value)
{
    json[key] = value;
};
