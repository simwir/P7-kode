include_directories(${PROJECT_SOURCE_DIR}/robot/include)

file(GLOB SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)


find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

if(${JSONCPP_LIBRARY_DIRS})
  link_libraries(${JSONCPP_LIBRARY_DIRS})
  include_directories("${JSONCPP_INCLUDE_DIRS}")
endif(${JSONCPP_LIBRARY_DIRS})


add_library(librobot ${SOURCES})

target_link_libraries(librobot
  tcp
  wbt-translator
  scheduling
  ${JSONCPP_LIBRARIES}
  scheduling
)

add_executable(robot src/main.cpp)
target_link_libraries(robot
  librobot
)

install(TARGETS robot DESTINATION bin)
