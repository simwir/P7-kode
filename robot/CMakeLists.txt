include_directories(${PROJECT_SOURCE_DIR}/robot/include)

file(GLOB SOURCES "src/*.cpp")


find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES}
  wbt-translator
  )
#include_directories("${JSONCPP_INCLUDE_DIRS}")

set(CMAKE_CXX_FLAGS "-fsanitize=address -g -fno-omit-frame-pointer -Wall")


add_executable(robot ${SOURCES})
target_link_libraries(robot
  tcp
  ${JSONCPP_LIBRARIES}
)

install(TARGETS robot DESTINATION bin)
