#ifndef QUERY_TEMPLATE_WRITER
#define QUERY_TEMPLATE_WRITER

#include <fstream>
#include <string>
#include "webots_parser.hpp"

const std::string TEMPL_START = "%¤";
const std::string TEMPL_END = "¤%";



bool instantiate_query_template(Parser &parser, std::istream &templ_input, std::ostream &output);

#endif
