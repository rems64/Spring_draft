# Root CMakeLists.txt

cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)

project(SpringAll)

add_subdirectory(lib/spdlog)
add_subdirectory(lib/utfconv)

add_subdirectory (Spring)
add_subdirectory (SpringEditor)