cmake_minimum_required(VERSION 3.5)
project(SpringEditor VERSION 1.0.0 LANGUAGES CXX)

set(SRCS
	"src/SpringEditor.cpp"
	"src/SpringEditorApplication.cpp"
)

add_executable(SpringEditor WIN32 ${SRCS})

target_include_directories(SpringEditor
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SRC_DIR}/src
)

# Warnings as errors
target_compile_options(SpringEditor PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W3 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

#find_package(SpringCore 1.0 REQUIRED)
target_link_libraries(SpringEditor SpringCore SpringMath SpringGraphics SpringAudio)

target_compile_features(SpringEditor PRIVATE cxx_std_17)


set_target_properties(SpringEditor PROPERTIES OUTPUT_NAME SpringEditor_${CMAKE_BUILD_TYPE})

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR} CACHE PATH "install here" FORCE)
install(TARGETS SpringEditor)