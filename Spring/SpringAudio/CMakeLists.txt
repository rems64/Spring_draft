project(springaudio VERSION 1.0.0 LANGUAGES CXX)

set(SRCS
		"src/SpringAudioModule.cpp"
		"src/SpringAudioApi.cpp"
		"src/SpringAudioApi_NativeWindows.cpp"
		"src/SpringAudioDevice_NativeWindows.cpp")

add_library(SpringAudio ${SRCS})
add_library(Spring::SpringAudio ALIAS SpringAudio)

target_include_directories(SpringAudio
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SRC_DIR}/src
)

target_precompile_headers(SpringAudio
	PUBLIC
		include/Spring/SpringAudio/pch.hpp
	PRIVATE
		src/pch.hpp
)

target_link_libraries(SpringAudio Spring::SpringCore)
