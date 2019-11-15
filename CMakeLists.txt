cmake_minimum_required(VERSION 3.10)
project(p7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "-g -fno-omit-frame-pointer -Wall")

include(ExternalProject)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(EXT_LIBRARY_INSTALL_PATH ${PROJECT_SOURCE_DIR}/build)

include_directories(
    ${PROJECT_SOURCE_DIR}/tcp/include
    ${PROJECT_SOURCE_DIR}/robot/include
    ${PROJECT_SOURCE_DIR}/scheduling/include
    ${PROJECT_SOURCE_DIR}/util/include
    ${PROJECT_SOURCE_DIR}/lib/catch
    ${PROJECT_SOURCE_DIR}/lib/spdlog
)

# JsonCpp
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

if(${JSONCPP_LIBRARY_DIRS})
  link_libraries(${JSONCPP_LIBRARY_DIRS})
  include_directories("${JSONCPP_INCLUDE_DIRS}")
endif(${JSONCPP_LIBRARY_DIRS})

# Libraries
add_subdirectory(lib)
add_subdirectory(tcp)
add_subdirectory(robot)
add_subdirectory(scheduling)

# Executables
add_subdirectory(tests)
add_subdirectory(port_discovery)
add_subdirectory(communication)
add_subdirectory(wbt-translator)
