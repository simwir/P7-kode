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

file(GLOB SOURCES "src/*.cpp")

link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(validator ${SOURCES})
target_link_libraries(validator
    utap
    xml2
)
install(TARGETS validator DESTINATION bin)
