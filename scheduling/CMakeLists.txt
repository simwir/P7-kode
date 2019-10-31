include_directories(${PROJECT_SOURCE_DIR}/include)

# set {SOURCES,PROJECT_LINK_LIBS} to expanded wildcards
file(GLOB SOURCES "src/*.cpp")
#file(GLOB PROJECT_LINK_LIBS "lib/*.so")
#file(GLOB PROJECT_STATIC_LINK_LIBS "lib/*.a")

add_library(scheduling ${SOURCES})
#install(FILES DESTINATION bin)

configure_file(waypoint_scheduling.xml ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/waypoint_scheduling.xml COPYONLY)
configure_file(waypoint_scheduling.q ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/waypoint_scheduling.q COPYONLY)
configure_file(station_scheduling.xml ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/station_scheduling.xml COPYONLY)
configure_file(station_scheduling.q ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/station_scheduling.q COPYONLY)
configure_file(get_eta.q ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/get_eta.q COPYONLY)

#file(COPY waypoint_scheduling.xml DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#file(COPY waypoint_scheduling.q DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#file(COPY station_scheduling.xml DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#file(COPY station_scheduling.q DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
