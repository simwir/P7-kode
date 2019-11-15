
include_directories(${PROJECT_SOURCE_DIR}/util/include)

file(GLOB SOURCES "src/*.cpp")

add_library(util ${SOURCES})

target_link_libraries(util
)
