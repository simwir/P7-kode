include_directories(${PROJECT_SOURCE_DIR}/unit_test/include)

file(GLOB SOURCES "src/*.cpp")

add_executable(unit_test ${SOURCES})
target_link_libraries(unit_test
    m # Math library for example
)
install(TARGETS unit_test DESTINATION bin)
