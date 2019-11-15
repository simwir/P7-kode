include_directories(${PROJECT_SOURCE_DIR}/tcp/include)

file(GLOB SOURCES "src/*.cpp")

add_library(tcp ${SOURCES})

add_executable(auto-responder ${SOURCES})
target_link_libraries(auto-responder
  tcp
  pthread
)

target_link_libraries(tcp
    m # Math library for example
)
