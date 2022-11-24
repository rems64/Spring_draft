cmake_minimum_required(VERSION 3.5)
project(SpringGraphics VERSION 1.0.0 LANGUAGES CXX)

set(SRCS
	"src/SpringGraphicsModule.cpp"
	"src/SpringWindow.cpp"
	"src/SpringGraphicsApi_Vulkan.cpp"
	"src/ISpringGraphicsApi.cpp"
	"src/SpringGraphicsDevice_Vulkan.cpp"
    )

if (WIN32)
	list(APPEND SRCS
        "src/SpringWindow_Win32.cpp"
        )
elseif(UNIX AND NOT APPLE)
    list(APPEND SRCS
        "src/SpringWindow_Glfw.cpp"
        )
endif ()

add_library(SpringGraphics ${SRCS})
#add_library(Spring::SpringGraphics ALIAS SpringGraphics)

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

# Warnings as errors
#target_compile_options(SpringGraphics PRIVATE
#		$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
#		$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
#		)



set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# add_subdirectory(lib/glfw)
# target_link_libraries(SpringGraphics glfw)
if (WIN32)
	set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
elseif(CMAKE_SYSTEM_NAME STREQUAL Linux)
	set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_XLIB_KHR)
elseif(CMAKE_SYSTEM_NAME STREQUAL Darwin)
	set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_MACOS_MVK)
endif()


#find_package(Vulkan REQUIRED)
add_subdirectory(lib/volk)

#set(VMA_STATIC_VULKAN_FUNCTIONS ON)
#set(VMA_DYNAMIC_VULKAN_FUNCTIONS OFF)
add_subdirectory(lib/VulkanMemoryAllocator)


target_link_libraries(SpringGraphics PRIVATE volk)
target_link_libraries(SpringGraphics PRIVATE VulkanMemoryAllocator)
#target_link_libraries(SpringGraphics Vulkan::Vulkan)
# target_link_libraries(SpringGraphics ${Vulkan_LIBRARY})

target_link_libraries(SpringGraphics PRIVATE Spring::SpringCore)
target_link_libraries(SpringGraphics PRIVATE utfconv)
target_link_libraries(SpringGraphics PRIVATE Dwmapi)


message("${Vulkan_LIBRARIES}")
message("${Vulkan_LIBRARY}")
message("${Vulkan_INCLUDE_DIR}")

target_compile_features(SpringGraphics PRIVATE cxx_std_20)