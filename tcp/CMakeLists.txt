include_directories(${PROJECT_SOURCE_DIR}/tcp/include)

file(GLOB SOURCES "src/*.cpp")

add_library(tcp ${SOURCES})

target_link_libraries(tcp
    m # Math library for example
)
