option (BUILD_DUMB "Disable sound support" OFF)
option (USE_CXIMAGE "Use CxImage instead of DevIL" OFF)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenAL REQUIRED)

if (NOT USE_CXIMAGE)
	find_package(DevIL REQUIRED)
endif ()

set (SMELT_GLFW_CPP_FILES
	gfx_glfw.cpp
	sfx_oal.cpp
	inp_glfw.cpp
	sys_glfw.cpp
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(
	${GLEW_INCLUDE_DIRS}
)

add_library(smelt STATIC ${SMELT_GLFW_CPP_FILES})

target_include_directories(smelt PUBLIC ../../include)

target_link_libraries(smelt glfw)

if (BUILD_DUMB)
	message (STATUS "")
	message (STATUS "BUILD_DUMB build option enabled")
	message (STATUS "")
	target_compile_definitions(smelt PRIVATE
		ENABLE_DUMB
	)
endif ()

if (USE_CXIMAGE)
	message (STATUS "")
	message (STATUS "USE_CXIMAGE build option enabled")
	message (STATUS "")
	target_compile_definitions(smelt PRIVATE
		USE_CXIMAGE
	)
endif ()
