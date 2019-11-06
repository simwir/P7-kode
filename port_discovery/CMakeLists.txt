include_directories(${PROJECT_SOURCE_DIR}/port_discovery/include)

file(GLOB SOURCES "src/*.cpp")

add_executable(port_discovery ${SOURCES})
target_link_libraries(port_discovery
    pthread
    tcp
)
install(TARGETS port_discovery DESTINATION bin)
