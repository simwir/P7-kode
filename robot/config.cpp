#include "config.hpp"

#include <fstream>
#include <string>
#include <vector>

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

    std::vector<int> result;

    for (Json::Value::const_iterator itr = json[key].begin(); itr != json[key].end(); itr++) {
        result.push_back(json[key][itr.index()].asInt());
    }

    return result;
}
