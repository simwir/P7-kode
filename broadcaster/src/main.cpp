#include <broadcaster/broadcaster.hpp>

int main(int argc, char **argv)
{
    broadcaster::Broadcaster broadcaster(0);
    broadcaster.start_broadcasting();
    return 0;
}