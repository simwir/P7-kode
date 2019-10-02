cmake_minimum_required(VERSION 3.10)
project(translator)

include_directories("${PROJECT_SOURCE_DIR}/include")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)

# set {SOURCES,PROJECT_LINK_LIBS} to expanded wildcards
file(GLOB SOURCES "src/*.cpp")
file(GLOB PROJECT_LINK_LIBS "lib/*.so")
file(GLOB PROJECT_STATIC_LINK_LIBS "lib/*.a")

link_directories(${PROJECT_SOURCE_DIR}/lib)

SET(CMAKE_EXE_LINKER_FLAGS "-lxml2")

add_executable(translator ${SOURCES})
target_link_libraries(translator ${PROJECT_LINK_LIBS})
target_link_libraries(translator ${PROJECT_STATIC_LINK_LIBS})
