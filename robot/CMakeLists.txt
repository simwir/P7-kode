include_directories(${PROJECT_SOURCE_DIR}/robot/include)

file(GLOB SOURCES "src/*.cpp")


find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

if(${JSONCPP_LIBRARY_DIRS})
  link_libraries(${JSONCPP_LIBRARY_DIRS})
  include_directories("${JSONCPP_INCLUDE_DIRS}")
endif(${JSONCPP_LIBRARY_DIRS})

add_executable(robot ${SOURCES})
target_link_libraries(robot
  tcp
  stdc++fs
  wbt-translator
  config
  ${JSONCPP_LIBRARIES}
  scheduling
)

install(TARGETS robot DESTINATION bin)

add_library(librobot ${SOURCES})
target_link_libraries(librobot
  tcp
  stdc++fs
  wbt-translator
  config
  ${JSONCPP_LIBRARIES}
  scheduling
)
