# Root CMakeLists.txt
cmake_minimum_required (VERSION 3.8)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g -OX")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

set(CMAKE_CXX_STANDARD 20)

project(Spring)

add_subdirectory(lib/spdlog)
add_subdirectory(lib/utfconv)

add_subdirectory (Spring)
add_subdirectory (SpringEditor)