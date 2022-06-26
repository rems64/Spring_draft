cmake_minimum_required(VERSION 3.5)
project(springgraphics VERSION 1.0.0 LANGUAGES CXX)

set(SRCS
	"src/SpringGraphicsDevice_Vk.cpp"
	"src/SpringGraphicsModule.cpp"
	"src/SpringWindow.cpp"
	"src/SpringWindow_Native.cpp"
	"src/SpringRenderer.cpp"
 "src/SpringGraphicsDevice_Gl.cpp" "src/SpringWindow_Glfw.cpp" "src/SpringGraphicsApi_Vulkan.cpp" "src/ISpringGraphicsApi.cpp")

add_library(SpringGraphics ${SRCS})
add_library(Spring::SpringGraphics ALIAS SpringGraphics)

target_include_directories(SpringGraphics
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SRC_DIR}/src
)

target_precompile_headers(SpringGraphics
	PUBLIC
		include/Spring/SpringGraphics/pch.hpp
	PRIVATE
		src/pch.hpp
)

find_package(Vulkan REQUIRED)
target_link_libraries(SpringGraphics Spring::SpringCore)
target_link_libraries(SpringGraphics Vulkan::Vulkan)

target_compile_features(SpringGraphics PRIVATE cxx_std_20)