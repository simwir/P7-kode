include_directories(
    ${PROJECT_SOURCE_DIR}/tests/include
    ${PROJECT_SOURCE_DIR}/webot/controllers/waypoints_runner/geo
)

file(GLOB SOURCES "src/*.cpp")

add_executable(tests
    ${SOURCES}
    ${PROJECT_SOURCE_DIR}/webot/controllers/waypoints_runner/geo/geo.cpp
)
target_link_libraries(tests
    m # Math library for example
)
install(TARGETS tests DESTINATION bin)
