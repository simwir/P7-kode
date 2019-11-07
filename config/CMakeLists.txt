include_directories(${PROJECT_SOURCE_DIR}/config/include)

file(GLOB SOURCES "src/*.cpp")

add_library(config SHARED ${SOURCES})

target_link_libraries(config
  ${JSONCPP_LIBRARIES}
)

install(TARGETS config DESTINATION bin)
