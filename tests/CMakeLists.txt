include_directories(${PROJECT_SOURCE_DIR}/tests/include)

file(GLOB SOURCES "src/*.cpp")

add_executable(tests ${SOURCES})
target_link_libraries(tests
    m # Math library for example
)
install(TARGETS tests DESTINATION bin)
