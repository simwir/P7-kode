include_directories(${PROJECT_SOURCE_DIR}/robot/include)

file(GLOB SOURCES "src/*.cpp")


find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)



add_executable(robot ${SOURCES})
target_link_libraries(robot
  tcp
  stdc++fs
  wbt-translator
  config
  ${JSONCPP_LIBRARIES}
)

install(TARGETS robot DESTINATION bin)
