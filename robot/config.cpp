#include "config.hpp"

#include <fstream>
#include <string>

robot::Config::Config() {}

robot::Config::Config(const std::string& file_path) {
  load_config_file(file_path);
}

void robot::Config::load_config_file(const std::string& file_path) {
  std::ifstream config_file(file_path);
  config_file >> json;
}

template <>
int robot::Config::get<int>(const std::string& key) {
  return json[key].asInt();
}

template <>
std::string robot::Config::get<std::string>(const std::string& key) {
  return json[key].asString();
}

template <>
double robot::Config::get<double>(const std::string& key) {
  return json[key].asDouble();
}
