#ifndef BROADCASTER_ROBOT_HPP
#define BROADCASTER_ROBOT_HPP

namespace robot{
    struct location {
        double x, y;
    };

    struct eta {
        int station;
        double time;
    };

    struct data {
        std::map<const int, location> location_map;
        std::map<const int, eta> eta_map;
        std::map<const int, std::vector<int>> next_stations;
    };
}

#endif //BROADCASTER_ROBOT_HPP
