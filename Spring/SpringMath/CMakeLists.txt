cmake_minimum_required(VERSION 3.5)
project(Spring VERSION 1.0.0 LANGUAGES CXX)

set(SRCS
        include/Spring/SpringMath/SpringVector3.hpp
        src/SpringVector3.cpp)


add_library(SpringMath ${SRCS})
#add_library(Spring::SpringGraphics ALIAS SpringGraphics)

target_include_directories(SpringMath
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SRC_DIR}/src
        )

target_precompile_headers(SpringMath
        PUBLIC
        include/Spring/SpringMath/pch.hpp
        PRIVATE
        src/pch.hpp
        )

# Warnings as errors
target_compile_options(SpringMath PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
        )

target_link_libraries(SpringMath Spring::SpringCore)