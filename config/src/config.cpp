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
#include "config/config.hpp"

#include <fstream>
#include <map>
#include <string>
#include <vector>

template <>
std::vector<double> config::convert_from_json<std::vector<double>>(const Json::Value &arr)
{
    if (arr.type() != Json::ValueType::arrayValue) {
        throw config::InvalidValueException{};
    }

    std::vector<double> result;

    for (auto elem : arr) {
        result.push_back(elem.asDouble());
    }

    return result;
}

template <>
std::vector<int> config::convert_from_json<std::vector<int>>(const Json::Value &arr)
{
    if (arr.type() != Json::ValueType::arrayValue) {
        throw config::InvalidValueException{};
    }

    std::vector<int> result;

    for (auto elem : arr) {
        result.push_back(elem.asInt());
    }

    return result;
}

template <>
std::vector<std::vector<int>>
config::convert_from_json<std::vector<std::vector<int>>>(const Json::Value &arr)
{
    if (arr.type() != Json::ValueType::arrayValue) {
        throw config::InvalidValueException{};
    }

    std::vector<std::vector<int>> result;

    for (auto elem : arr) {
        result.push_back(config::convert_from_json<std::vector<int>>(elem));
    }

    return result;
}

template <>
std::vector<bool> config::convert_from_json<std::vector<bool>>(const Json::Value &arr)
{
    if (arr.type() != Json::ValueType::arrayValue) {
        throw config::InvalidValueException{};
    }

    std::vector<bool> result;

    for (auto elem : arr) {
        result.push_back(elem.asBool());
    }

    return result;
}

config::Config::Config(const std::string &file_path)
{
    load_from_file(file_path);
}

void config::Config::load_from_file(const std::string &file_path)
{
    std::ifstream config_file{file_path};
    config_file >> json;
}

void config::Config::write_to_file(const std::string &file_path)
{
    std::ofstream config_file{file_path};
    config_file << json;
}

template <>
int config::Config::get<int>(const std::string &key)
{
    if (!json.isMember(key)) {
        throw config::InvalidKeyException{key};
    };

    return json[key].asInt();
}

template <>
std::string config::Config::get<std::string>(const std::string &key)
{
    if (!json.isMember(key)) {
        throw config::InvalidKeyException{key};
    };

    return json[key].asString();
}

template <>
double config::Config::get<double>(const std::string &key)
{
    if (!json.isMember(key)) {
        throw config::InvalidKeyException{key};
    };

    return json[key].asDouble();
}

template <>
std::vector<int> config::Config::get<std::vector<int>>(const std::string &key)
{
    if (!json.isMember(key)) {
        throw config::InvalidKeyException{key};
    }

    return config::convert_from_json<std::vector<int>>(json[key]);
}

template <>
std::vector<std::vector<int>>
config::Config::get<std::vector<std::vector<int>>>(const std::string &key)
{
    if (!json.isMember(key)) {
        throw config::InvalidKeyException{key};
    }

    return config::convert_from_json<std::vector<std::vector<int>>>(json[key]);
}

template <>
std::vector<bool> config::Config::get<std::vector<bool>>(const std::string &key)
{
    if (!json.isMember(key)) {
        throw config::InvalidKeyException{key};
    }

    return config::convert_from_json<std::vector<bool>>(json[key]);
}

template <>
std::vector<double> config::Config::get<std::vector<double>>(const std::string &key)
{
    if (!json.isMember(key)) {
        throw config::InvalidKeyException{key};
    }

    return config::convert_from_json<std::vector<double>>(json[key]);
}

template <>
std::map<int, std::vector<int>>
config::Config::get<std::map<int, std::vector<int>>>(const std::string &key)
{
    if (!json.isMember(key)) {
        throw config::InvalidKeyException{key};
    }

    if (json[key].type() != Json::ValueType::objectValue) {
        throw config::InvalidValueException{};
    }

    std::map<int, std::vector<int>> result;
    Json::Value obj_map = json[key];

    for (Json::Value::const_iterator itr = obj_map.begin(); itr != obj_map.end(); itr++) {
        std::vector<int> value = config::convert_from_json<std::vector<int>>(*itr);
        result.insert({itr.key().asInt(), value});
    }

    return result;
}
