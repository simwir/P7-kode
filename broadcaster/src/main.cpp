#include <broadcaster/broadcaster.hpp>

std::vector<std::string> split_message(const std::string message){
    std::string start = message.substr(0, message.find(','));
    std::string end = message.substr(message.find(',') + 1);
    return std::vector<std::string> {start, end};
}

int main(int argc, char **argv)
{
    broadcaster::Broadcaster broadcaster(0);
    broadcaster.start_broadcasting();

    return 0;
}