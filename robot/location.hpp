#include <json/json.h>

#include <map>

namespace robot {
struct Location {
  double x, y;
  Json::Value to_json() const;
  static Location from_json(const Json::Value &json);
};

class LocationMap {
  std::map<int, Location> locations;

 public:
  Json::Value to_json() const;
  Location &operator[](int index);
  static LocationMap from_json(const Json::Value &json);
};
}  // namespace robot
