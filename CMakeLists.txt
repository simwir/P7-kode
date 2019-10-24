cmake_minimum_required(VERSION 3.10)
project(master)

include_directories("${PROJECT_SOURCE_DIR}/include")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS  "-g -fsanitize=address -fno-omit-frame-pointer -Wall")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Following flags helps with dead code elimination.
# More info can found at http://stackoverflow.com/a/10809541

include_directories(
	# Root
	${CMAKE_SOURCE_DIR}

	# Libraries
	scheduling/include
)

add_subdirectory(scheduling)

# set {SOURCES,PROJECT_LINK_LIBS} to expanded wildcards
file(GLOB SOURCES "src/*.cpp")

#file(GLOB PROJECT_LINK_LIBS "lib/*.so")
#file(GLOB PROJECT_STATIC_LINK_LIBS "lib/*.a")

add_executable(master ${SOURCES})
target_link_libraries(master scheduling)
install(TARGETS master DESTINATION bin)
