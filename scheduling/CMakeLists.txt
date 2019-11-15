include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_library(scheduling ${SOURCES})
target_link_libraries(scheduling
    pthread
    stdc++fs
    ${JSONCPP_LIBRARIES}
)

configure_file(waypoint_scheduling.xml ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/waypoint_scheduling.xml COPYONLY)
configure_file(waypoint_scheduling.q ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/waypoint_scheduling.q COPYONLY)
configure_file(station_scheduling.xml ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/station_scheduling.xml COPYONLY)
configure_file(station_scheduling.q ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/station_scheduling.q COPYONLY)
configure_file(get_eta.q ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/get_eta.q COPYONLY)

# Testing main function
add_executable(test_scheduler ${SOURCES} src/main.cpp)
target_link_libraries(test_scheduler
    scheduling
)
install(TARGETS test_scheduler DESTINATION bin)
