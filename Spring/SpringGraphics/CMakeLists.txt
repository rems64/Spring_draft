cmake_minimum_required(VERSION 3.5)
project(springgraphics VERSION 1.0.0 LANGUAGES CXX)

set(SRCS
	
	"src/SpringGraphicsDevice_Vk.cpp"
)

add_library(SpringGraphics ${SRCS})

target_include_directories(SpringGraphics
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SRC_DIR}/src
)

#find_package(SpringCore 1.0 REQUIRED)
find_package(Vulkan REQUIRED)
target_link_libraries(SpringGraphics SpringCore)
target_link_libraries(SpringGraphics Vulkan::Vulkan)

target_compile_features(SpringGraphics PRIVATE cxx_std_17)