cmake_minimum_required(VERSION 3.5)
project(springgraphics VERSION 1.0.0 LANGUAGES CXX)

set(SRCS
	
	"src/SpringGraphicsModule.cpp"
	"src/SpringWindow.cpp"
	"src/SpringWindow_Win32.cpp"
	"src/SpringWindow_Glfw.cpp"
	"src/SpringGraphicsApi_Vulkan.cpp"
	"src/ISpringGraphicsApi.cpp"
	"src/SpringGraphicsDevice_Vulkan.cpp"
)


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

target_compile_definitions(SpringGraphics
	PRIVATE	SPRING_BUILD_VK
	PUBLIC SPRING_BUILD_VK
)


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# add_subdirectory(lib/glfw)
# target_link_libraries(SpringGraphics glfw)

find_package(Vulkan REQUIRED)

target_link_libraries(SpringGraphics Spring::SpringCore)
target_link_libraries(SpringGraphics Vulkan::Vulkan)
target_link_libraries(SpringGraphics utfconv)
target_link_libraries(SpringGraphics Dwmapi)
# target_link_libraries(SpringGraphics ${Vulkan_LIBRARY})


message("${Vulkan_LIBRARIES}")
message("${Vulkan_LIBRARY}")
message("${Vulkan_INCLUDE_DIR}")

target_compile_features(SpringGraphics PRIVATE cxx_std_20)