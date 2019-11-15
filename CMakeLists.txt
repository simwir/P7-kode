cmake_minimum_required(VERSION 3.10)
project(p7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "-g -fno-omit-frame-pointer -Wall -Werror -m32")

include(ExternalProject)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(EXT_LIBRARY_INSTALL_PATH ${PROJECT_SOURCE_DIR}/build)

set(JSONCPP_ARCHIVE ${PROJECT_SOURCE_DIR}/lib/jsoncpp/libjsoncpp.a)

include_directories(
    ${PROJECT_SOURCE_DIR}/lib/jsoncpp/include
    ${PROJECT_SOURCE_DIR}/tcp/include
    ${PROJECT_SOURCE_DIR}/robot/include
    ${PROJECT_SOURCE_DIR}/wbt-translator/include
    ${PROJECT_SOURCE_DIR}/scheduling/include
    ${PROJECT_SOURCE_DIR}/util/include
    ${PROJECT_SOURCE_DIR}/config/include
)

# Libraries
add_subdirectory(tcp)
add_subdirectory(config)
add_subdirectory(robot)
add_subdirectory(scheduling)

# Executables
add_subdirectory(tests)
add_subdirectory(port_discovery)
add_subdirectory(communication)
add_subdirectory(wbt-translator)
