project (smelt)

cmake_minimum_required(VERSION 3.10)

option (BUILD_DUMB "Disable sound support" OFF)
option (BUILD_EXTENSIONS "Build extensions" ON)
option (USE_CXIMAGE "Use CxImage instead of DevIL" OFF)
option (BUILD_EXAMPLE "Build example" ON)

add_subdirectory(smelt/glfw)

if (BUILD_EXTENSIONS)
	add_subdirectory(extensions)
endif ()

if (BUILD_EXAMPLE)
	add_subdirectory(examples)
endif ()
