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
#include "util/log.hpp"

#include <fstream>
#include <map>
#include <string>
#include <vector>

extern Log log;

template <>
std::pair<std::string, int> config::convert_from_json<std::pair<std::string, int>>(const Json::Value &arr)
{
    if (arr.type() != Json::ValueType::arrayValue) {
        throw config::InvalidValueException{"convert_from_json<std::pair<std::string, int>>"};
    }

    return std::make_pair(arr[0].asString(), arr[1].asInt());
}

template <>
std::vector<double> config::convert_from_json<std::vector<double>>(const Json::Value &arr)
{
    if (arr.type() != Json::ValueType::arrayValue) {
        throw config::InvalidValueException{"convert_from_json<std::vector<double>>"};
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
        throw config::InvalidValueException{"convert_from_json<std::vector<int>>"};
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
        throw config::InvalidValueException{"<std::vector<std::vector<int>>>"};
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
        throw config::InvalidValueException{"convert_from_json<std::vector<bool>>"};
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
        throw config::InvalidValueException{"get<std::map<int, std::vector<int>>>"};
    }

    std::map<int, std::vector<int>> result;
    Json::Value obj_map = json[key];

    for (Json::Value::const_iterator itr = obj_map.begin(); itr != obj_map.end(); itr++) {
        std::vector<int> value = config::convert_from_json<std::vector<int>>(*itr);
        result.insert({itr.key().asInt(), value});
    }

    return result;
}

template <>
std::vector<std::vector<std::pair<std::string, int>>>
config::Config::get<std::vector<std::vector<std::pair<std::string, int>>>>(const std::string &key) {
    if (!json.isMember(key)) {
        throw config::InvalidKeyException{key};
    }

    std::vector<std::vector<std::pair<std::string, int>>> result;
    for (const auto& json_row : json[key]) {
        if (json_row.type() != Json::ValueType::arrayValue) {
            throw config::InvalidValueException{"get<std::vector<std::vector<std::pair<std::string, int>>>>"};
        }

        std::vector<std::pair<std::string, int>> row;
        for (const auto& elem : json_row) {
            row.push_back(convert_from_json<std::pair<std::string, int>>(elem));
        }
        result.push_back(row);
    }

    return result;
}

template <>
std::vector<std::vector<int>> config::Config::get<std::vector<std::vector<int>>>(const std::string &key1, const std::string &key2) {
    if (!json.isMember(key1)) {
        throw config::InvalidKeyException{key1};
    }

    std::vector<std::vector<int>> result;
    for (const auto& level1 : json[key1]) {
        if (level1.type() != Json::ValueType::objectValue) {
            throw config::InvalidValueException{"get<std::vector<std::vector<int>>> level1"};
        }

        if (!level1.isMember(key2)) {
            throw config::InvalidKeyException{key2};
        }

        if (level1[key2].type() != Json::ValueType::objectValue && level1[key2].type() != Json::ValueType::arrayValue) {
            throw config::InvalidValueException{"get<std::vector<std::vector<int>>> level2"};
        }

        std::vector<int> row;
        for (const auto& elem : level1[key2]) {
            row.push_back(elem.asInt());
        }
        result.push_back(row);
    }

    return result;
}

template <>
std::vector<double> config::Config::get<std::vector<double>>(const std::string &key1, const std::string &key2) {
    if (!json.isMember(key1)) {
        throw config::InvalidKeyException{key1};
    }

    std::vector<double> result;
    for (const auto& level1 : json[key1]) {
        if (level1.type() != Json::ValueType::objectValue) {
            throw config::InvalidValueException{"get<std::vector<double>>"};
        }

        if (!level1.isMember(key2)) {
            throw config::InvalidKeyException{key2};
        }
        result.push_back(level1[key2].asDouble());
    }

    return result;
}

template <>
std::vector<std::vector<std::pair<std::string, int>>> config::Config::get<std::vector<std::vector<std::pair<std::string, int>>>>(const std::string &key1, const std::string &key2) {
  if (!json.isMember(key1)) {
      throw config::InvalidKeyException{key1};
  }

  std::vector<std::vector<std::pair<std::string, int>>> result;
  for (const auto& level1 : json[key1]) {
      if (level1.type() != Json::ValueType::objectValue) {
          throw config::InvalidValueException{"get<std::vector<std::vector<std::pair<std::string, int>>>> level1"};
      }

      if (!level1.isMember(key2)) {
          throw config::InvalidKeyException{key2};
      }

      if (level1[key2].type() != Json::ValueType::objectValue && level1[key2].type() != Json::ValueType::arrayValue) {
          throw config::InvalidValueException{"get<std::vector<std::vector<std::pair<std::string, int>>>> level2"};
      }

      std::vector<std::pair<std::string, int>> row;
      for (const auto& elem : level1[key2]) {
          row.push_back(std::make_pair(elem["type"].asString(), elem["value"].asInt()));
      }
      result.push_back(row);
  }

  return result;
}

int config::Config::getSize(const std::string &key)
{
    if (!json.isMember(key)) {
        throw config::InvalidKeyException{key};
    }

    if (json[key].type() == Json::ValueType::arrayValue) {
        return json[key].size();
    }
    else if (json[key].type() == Json::ValueType::objectValue) {
        return json[key].size();
    }
    else {
        throw config::InvalidValueException{"getSize " + key};
    }
}
