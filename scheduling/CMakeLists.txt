include_directories(${PROJECT_SOURCE_DIR}/include)

# set {SOURCES,PROJECT_LINK_LIBS} to expanded wildcards
file(GLOB SOURCES "src/*.cpp")
#file(GLOB PROJECT_LINK_LIBS "lib/*.so")
#file(GLOB PROJECT_STATIC_LINK_LIBS "lib/*.a")

add_library(scheduling ${SOURCES})
#install(FILES DESTINATION bin)
file(COPY waypoint_scheduling.xml DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY waypoint_scheduling.q DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})