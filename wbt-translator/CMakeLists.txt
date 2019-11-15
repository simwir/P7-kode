include_directories(${PROJECT_SOURCE_DIR}/wbt-translator/include)

file(GLOB SOURCES "src/*.cpp")

add_library(wbt-translator ${SOURCES})

add_executable(webots-parser ${SOURCES})

target_link_libraries(
  wbt-translator
)
