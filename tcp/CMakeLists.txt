cmake_minimum_required(VERSION 3.10)
project(translator)

include_directories("${PROJECT_SOURCE_DIR}/include")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(EXT_LIBRARY_INSTALL_PATH ${PROJECT_SOURCE_DIR}/build)

#include_directories(${PROJECT_SOURCE_DIR}/build/include)

# set {SOURCES,PROJECT_LINK_LIBS} to expanded wildcards
file(GLOB SOURCES "*.cpp")
#file(GLOB PROJECT_LINK_LIBS "lib/*.so")
#file(GLOB PROJECT_STATIC_LINK_LIBS "lib/*.a")

#link_directories(${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_EXE_LINKER_FLAGS "-lpthread")
set(CMAKE_CXX_FLAGS "-fsanitize=address -fno-omit-frame-pointer")

add_executable(translator ${SOURCES} portService.cpp)
#target_link_libraries(translator utap)
install(TARGETS translator DESTINATION bin)

#target_link_libraries(translator ${PROJECT_LINK_LIBS})
#target_link_libraries(translator ${PROJECT_STATIC_LINK_LIBS})
