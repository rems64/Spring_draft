#if(MSVC)
#  target_compile_options(Test1 PUBLIC "/Zi")
#  target_link_options(Test1 PUBLIC "/INCREMENTAL")
#endif()
cmake_minimum_required(VERSION 3.5)
project(springcore VERSION 1.0.0 LANGUAGES CXX)

set(SRCS
	
	"src/SpringMisc.cpp"
	"src/SpringApplication.cpp"
 "src/SpringModule.cpp")

add_library(SpringCore STATIC ${SRCS})
add_library(Spring::SpringCore ALIAS SpringCore)
target_include_directories(SpringCore
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SRC_DIR}/src
)

target_precompile_headers(SpringCore
	PUBLIC
		include/Spring/SpringCore/pch.hpp
	PRIVATE
		src/pch.hpp
)

target_compile_features(SpringCore PRIVATE cxx_std_17)