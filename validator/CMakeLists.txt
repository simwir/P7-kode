cmake_minimum_required(VERSION 3.10)
project(translator)

include_directories("${PROJECT_SOURCE_DIR}/include")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(EXT_LIBRARY_INSTALL_PATH ${PROJECT_SOURCE_DIR}/build)

include(ExternalProject)
ExternalProject_Add(libutap
  URL http://people.cs.aau.dk/~marius/utap/libutap-0.94.tar.gz
  PREFIX ${EXT_LIBRARY_INSTALL_PATH}/libs/libutap-0.94
  #SOURCE_DIR ${EXT_LIBRARY_INSTALL_PATH}/libutap-0.94
  INSTALL_DIR ${EXT_LIBRARY_INSTALL_PATH}
  CONFIGURE_COMMAND ${EXT_LIBRARY_INSTALL_PATH}/libs/libutap-0.94/src/libutap/configure --prefix=${EXT_LIBRARY_INSTALL_PATH}
  BUILD_COMMAND make
)

ExternalProject_Get_Property(libutap install_dir)

add_library(utap STATIC IMPORTED)
set_property(TARGET utap PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libutap.a)

add_dependencies(utap libutap)

include_directories(${PROJECT_SOURCE_DIR}/build/include)

# set {SOURCES,PROJECT_LINK_LIBS} to expanded wildcards
file(GLOB SOURCES "src/*.cpp")
#file(GLOB PROJECT_LINK_LIBS "lib/*.so")
#file(GLOB PROJECT_STATIC_LINK_LIBS "lib/*.a")

link_directories(${PROJECT_SOURCE_DIR}/lib)

SET(CMAKE_EXE_LINKER_FLAGS "-lxml2")

add_executable(translator ${SOURCES})
target_link_libraries(translator utap)
install(TARGETS translator DESTINATION bin)

#target_link_libraries(translator ${PROJECT_LINK_LIBS})
#target_link_libraries(translator ${PROJECT_STATIC_LINK_LIBS})
