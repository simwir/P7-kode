#include <broadcaster/broadcaster.hpp>
#include <iostream>

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cout << "Please specify what port the broadcaster is run on. Use 0 for OS to assign any available port." << std::endl;
        exit(0);
    }
    broadcaster::Broadcaster broadcaster(std::stoi(argv[1]));
    broadcaster.start_broadcasting();
    return 0;
}
