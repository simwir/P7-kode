include_directories(${PROJECT_SOURCE_DIR}/tests/include)

file(GLOB SOURCES "src/*.cpp")

add_library(board_driver ${SOURCES})

target_link_libraries(board_driver
    m # Math library for example
)
