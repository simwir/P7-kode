include_directories(${PROJECT_SOURCE_DIR}/order/include)

file (GLOB SOURCES "src/*.cpp")

add_executable(order ${SOURCES})
target_link_libraries(order
  jsoncpp
  pthread
  tcp
)
install(TARGETS order DESTINATION bin)
