#ifndef XML_GENERATOR_HPP
#define XML_GENERATOR_HPP

#include "webots_parser.hpp"

#include <iostream>
#include <regex>

void generate_xml(const AST& ast, int number_of_robots, std::ostream &os, std::istream &is);

#endif
