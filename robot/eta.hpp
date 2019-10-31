#ifndef ETA_HPP
#define ETA_HPP

#include <json/json.h>

namespace robot {
struct ETA {
    int station_id;
    double time;
    Json::Value to_json() const;
    static ETA from_json(const Json::Value &json);
};

struct ETAMap {
    std::map<int, std::vector<ETA>> eta_map;
    Json::Value to_json() const;
    std::vector<ETA> &operator[](int index);
    static ETAMap from_json(const Json::Value &json);
};
} // namespace robot

#endif
