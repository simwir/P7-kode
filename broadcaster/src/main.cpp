#include <broadcaster/broadcaster.hpp>

std::vector<std::string> split_message(const std::string message){
    std::string start = message.substr(0, message.find(','));
    std::string end = message.substr(message.find(',') + 1);
    return std::vector<std::string> {start, end};
}

int main(int argc, char **argv)
{
    /*std::string teststring = "#|post_robot_location,{\"id\":1,\"location\":{\"x\":1.1, \"y\":2.2},\"station_plan\":[1,2,3,4,5],\"waypoint_plan\":[3,1,2,3,5,6],\"eta\":2.42}|#";
    std::vector<std::string> result = split_message(teststring);
    robot::Info info = robot::Info::from_json(result[0]);*/
    broadcaster::Broadcaster broadcaster(0);
    broadcaster.start_broadcasting();

    return 0;
}