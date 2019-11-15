include_directories(${PROJECT_SOURCE_DIR}/communication/include)

file(GLOB SOURCES "src/*.cpp")

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

add_executable(communication ${SOURCES})
target_link_libraries(communication
    pthread
    tcp
    ${JSONCPP_ARCHIVE}
    librobot
)
install(TARGETS communication DESTINATION bin)
