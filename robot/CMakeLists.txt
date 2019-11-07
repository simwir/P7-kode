include_directories(${PROJECT_SOURCE_DIR}/robot/include)

file(GLOB SOURCES "src/*.cpp")

add_library(robot ${SOURCES})

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

if(${JSONCPP_LIBRARY_DIRS})
  link_libraries(${JSONCPP_LIBRARY_DIRS})
  include_directories("${JSONCPP_INCLUDE_DIRS}")
endif(${JSONCPP_LIBRARY_DIRS})

target_link_libraries(robot
  ${JSONCPP_LIBRARIES}
)
