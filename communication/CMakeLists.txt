include_directories(${PROJECT_SOURCE_DIR}/communication/include)

file(GLOB SOURCES "src/*.cpp")

add_executable(broadcaster ${SOURCES})
target_link_libraries(broadcaster
    pthread
    tcp
    ${JSONCPP_LIBRARIES}
    robot
)
install(TARGETS broadcaster DESTINATION bin)
