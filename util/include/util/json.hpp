#ifndef JSON_HEADER_WRAPPER_HPP
#define JSON_HEADER_WRAPPER_HPP

#if __has_include("json/json.h")
#include <json/json.h>
#else
#include <jsoncpp/json/json.h>
#endif

#endif
