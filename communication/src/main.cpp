#include "communication/com-module.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cout << "Please specify what port the communication module  is run on. Use 0 for OS "
                     "to assign any available port."
                  << std::endl;
        exit(0);
    }
    communication::ComModule com_module(std::stoi(argv[1]));
    com_module.start_broadcasting();
    return 0;
}
