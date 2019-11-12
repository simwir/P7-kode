cmake_minimum_required(VERSION 3.10)
project(p7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)

include(ExternalProject)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(EXT_LIBRARY_INSTALL_PATH ${PROJECT_SOURCE_DIR}/build)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -m32")

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

#link_directories(${JSONCPP_LIBRARY_DIRS})
link_directories(lib)

include_directories(
    ${JSONCPP_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/tcp/include
    ${PROJECT_SOURCE_DIR}/robot/include
    ${PROJECT_SOURCE_DIR}/wbt-translator/include
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/scheduling/include
    ${PROJECT_SOURCE_DIR}/util/include
    ${PROJECT_SOURCE_DIR}/config/include
)

# Libraries
add_subdirectory(tcp)
add_subdirectory(config)
add_subdirectory(scheduling)

# Executables
add_subdirectory(tests)
add_subdirectory(port_discovery)
add_subdirectory(wbt-translator)
add_subdirectory(robot)
