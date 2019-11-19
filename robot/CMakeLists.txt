include_directories(${PROJECT_SOURCE_DIR}/robot/include)

file(GLOB SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_library(librobot ${SOURCES})
target_link_libraries(librobot
  tcp
  stdc++fs
  wbt-translator
  config
  ${JSONCPP_ARCHIVE}
  scheduling
)

add_executable(robot src/main.cpp)
target_link_libraries(robot
  librobot
)

install(TARGETS robot DESTINATION bin)
