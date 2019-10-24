cmake_minimum_required(VERSION 3.10)
project(port_service)

include_directories("${PROJECT_SOURCE_DIR}/include")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(EXT_LIBRARY_INSTALL_PATH ${PROJECT_SOURCE_DIR}/build)

file(GLOB SOURCES "*.cpp")

set(CMAKE_EXE_LINKER_FLAGS "-lpthread")
set(CMAKE_CXX_FLAGS "-fsanitize=address -fno-omit-frame-pointer")

add_executable(port_service ${SOURCES} port_service.cpp)

install(TARGETS port_service DESTINATION bin)