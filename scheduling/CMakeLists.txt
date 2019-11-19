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

include_directories(${PROJECT_SOURCE_DIR}/include)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

# set {SOURCES,PROJECT_LINK_LIBS} to expanded wildcards
file(GLOB SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
#file(GLOB PROJECT_LINK_LIBS "lib/*.so")
#file(GLOB PROJECT_STATIC_LINK_LIBS "lib/*.a")

add_library(scheduling ${SOURCES})
target_link_libraries(scheduling
    pthread
    stdc++fs
    ${JSONCPP_ARCHIVE}
)
#install(FILES DESTINATION bin)

configure_file(waypoint_scheduling.xml ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/waypoint_scheduling.xml COPYONLY)
configure_file(waypoint_scheduling.q ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/waypoint_scheduling.q COPYONLY)
configure_file(station_scheduling.xml ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/station_scheduling.xml COPYONLY)
configure_file(station_scheduling.q ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/station_scheduling.q COPYONLY)
configure_file(get_eta.q ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/get_eta.q COPYONLY)

add_executable(test_scheduler src/main.cpp)
target_link_libraries(test_scheduler
    scheduling
)
install(TARGETS test_scheduler DESTINATION bin)
