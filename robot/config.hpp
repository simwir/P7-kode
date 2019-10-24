#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <json/json.h>

#include <string>

namespace robot {
class Config {
  Json::Value json;

 public:
  Config();
  Config(const std::string& file_path);
  void load_config_file(const std::string& file_path);

  template <typename T>
  T get(const std::string& key);
};
}  // namespace robot

#endif
