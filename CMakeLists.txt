# Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler
# Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
# associated documentation files (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge, publish, distribute,
# sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or
# substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
# NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
# OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER clang++)

project(p7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)
set(CMAKE_CXX_FLAGS "-g -fno-omit-frame-pointer -Wall")

include(ExternalProject)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(EXT_LIBRARY_INSTALL_PATH ${PROJECT_SOURCE_DIR}/build)

# set(JSONCPP_ARCHIVE ${PROJECT_SOURCE_DIR}/lib/jsoncpp/libjsoncpp.a)

include_directories(
    ${PROJECT_SOURCE_DIR}/lib/jsoncpp/include
    ${PROJECT_SOURCE_DIR}/tcp/include
    ${PROJECT_SOURCE_DIR}/robot/include
    ${PROJECT_SOURCE_DIR}/wbt-translator/include
    ${PROJECT_SOURCE_DIR}/scheduling/include
    ${PROJECT_SOURCE_DIR}/util/include
    ${PROJECT_SOURCE_DIR}/config/include
    ${PROJECT_SOURCE_DIR}/order/include
)

# Libraries
add_subdirectory(lib)
add_subdirectory(tcp)
add_subdirectory(config)
add_subdirectory(scheduling)
add_subdirectory(robot)
add_subdirectory(order)

# Executables
add_subdirectory(tests)
add_subdirectory(port_discovery)
add_subdirectory(communication)
add_subdirectory(wbt-translator)
