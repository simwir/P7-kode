cmake_minimum_required(VERSION 3.10)
project(p7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)

include(ExternalProject)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(EXT_LIBRARY_INSTALL_PATH ${PROJECT_SOURCE_DIR}/build)

include_directories(
    ${PROJECT_SOURCE_DIR}/tcp/include
)

# Libraries
add_subdirectory(tcp)

# Executables
add_subdirectory(tests)
add_subdirectory(port_discovery)
