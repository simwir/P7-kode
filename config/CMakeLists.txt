include_directories(${PROJECT_SOURCE_DIR}/config/include)

file(GLOB SOURCES "src/*.cpp")

add_library(config SHARED ${SOURCES})

target_link_libraries(config
  jsoncpp_lib
)
target_compile_options(config PRIVATE -Werror)

#set_target_properties(config PROPERTIES COMPILE_FLAGS "-fpic")
set_target_properties(config PROPERTIES LINK_FLAGS "-rdynamic")

install(TARGETS config DESTINATION bin)
