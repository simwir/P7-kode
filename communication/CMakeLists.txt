include_directories(${PROJECT_SOURCE_DIR}/communication/include)

file(GLOB SOURCES "src/*.cpp")

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

add_executable(com_module ${SOURCES})
target_link_libraries(com_module
    pthread
    tcp
    ${JSONCPP_LIBRARIES}
    robot
)
install(TARGETS com_module DESTINATION bin)
