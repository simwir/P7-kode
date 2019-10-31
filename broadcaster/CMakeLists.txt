include_directories(${PROJECT_SOURCE_DIR}/broadcaster/include)

file(GLOB SOURCES "src/*.cpp")

add_executable(broadcaster ${SOURCES})
target_link_libraries(broadcaster
    pthread
    tcp
    jsoncpp
)
install(TARGETS broadcaster DESTINATION bin)
