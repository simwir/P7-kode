include_directories(${PROJECT_SOURCE_DIR}/broadcaster/include)

file(GLOB SOURCES "src/*.cpp")

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

add_executable(broadcaster ${SOURCES})
target_link_libraries(broadcaster
    pthread
    tcp
    ${JSONCPP_LIBRARIES}
    robot
)
install(TARGETS broadcaster DESTINATION bin)
