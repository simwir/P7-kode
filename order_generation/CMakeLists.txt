include_directories(${PROJECT_SOURCE_DIR}/order_generation/include)

file (GLOB SOURCES "src/*.cpp")

target_link_libraries(order_generator)
