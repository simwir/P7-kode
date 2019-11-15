include_directories(${PROJECT_SOURCE_DIR}/robot/include)

file(GLOB SOURCES "src/*.cpp")

add_library(robot ${SOURCES})

target_link_libraries(robot
  ${JSONCPP_LIBRARIES}
  scheduling
)
