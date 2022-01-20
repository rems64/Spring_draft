#if(MSVC)
#  target_compile_options(Test1 PUBLIC "/Zi")
#  target_link_options(Test1 PUBLIC "/INCREMENTAL")
#endif()
cmake_minimum_required(VERSION 3.5)
project(springcore VERSION 1.0.0 LANGUAGES CXX)

set(SRCS
	"src/SpringTime.cpp"
	"src/SpringMisc.cpp"
)

add_library(SpringCore STATIC ${SRCS})
target_include_directories(SpringCore
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SRC_DIR}/src
)

target_compile_features(SpringCore PRIVATE cxx_std_17)