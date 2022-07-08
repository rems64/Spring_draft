cmake_minimum_required(VERSION 3.5)
project(springeditor VERSION 1.0.0 LANGUAGES CXX)

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
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

#find_package(SpringCore 1.0 REQUIRED)
target_link_libraries(SpringEditor SpringCore)
target_link_libraries(SpringEditor SpringGraphics)

target_compile_features(SpringEditor PRIVATE cxx_std_17)