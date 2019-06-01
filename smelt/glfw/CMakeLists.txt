option(BUILD_DUMB "Disable sound support" OFF)

find_package(PkgConfig REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenAL REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)

set (SMELT_GLFW_CPP_FILES
	gfx_glfw.cpp
	sfx_oal.cpp
	inp_glfw.cpp
	sys_glfw.cpp
)

include_directories(
	${GLFW_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}
)

add_library(smelt STATIC ${SMELT_GLFW_CPP_FILES})

if (BUILD_DUMB)
	message (STATUS "")
	message (STATUS "BUILD_DUMB build option enabled")
	message (STATUS "")
	target_compile_definitions(smelt PRIVATE
		ENABLE_DUMB
	)
endif ()
