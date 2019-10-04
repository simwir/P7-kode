

#include "webots_parser.hpp"


std::string Parser::read_token()
{
    std::string token;
    if (!(stream >> token)) {
        if (stream.eof()) {
        }
    }
    return token;
}
