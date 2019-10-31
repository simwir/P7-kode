#include <port_discovery/port_service.hpp>

int main(int argc, char **argv)
{
    port_discovery::PortService port_service(4444);
    port_service.start();
}