# SPDX-License-Identifier: GPL-2.0-or-later

set(INC
	"include/"
)

set(SRC
	src/utfconv.c
	include/utfconv.h
)

set(LIB
)

# This is odd but leave it for now...
# Why have win32 check here? - this is only used for windows.
# ... because one day we might want to use it on other platforms.
if(WIN32)
	list(APPEND SRC
		src/utf_winfunc.c
		include/utf_winfunc.h
	)
endif()

# blender_add_lib(bf_intern_utfconv "${SRC}" "${INC}" "${INC_SYS}" "${LIB}")
add_library(utfconv "${SRC}" "${INC}" "${LIB}")

target_include_directories(utfconv
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SRC_DIR}/
)