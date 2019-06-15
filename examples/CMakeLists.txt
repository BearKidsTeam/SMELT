option (USE_CXIMAGE "Use CxImage instead of DevIL" OFF)

find_package(glfw3 3.2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(Freetype REQUIRED)
find_package(ZLIB REQUIRED)

if (NOT USE_CXIMAGE)
        find_package(DevIL REQUIRED)
endif ()

if (NOT BUILD_DUMB)
	find_package(PkgConfig REQUIRED)
	find_package(OpenAL REQUIRED)
	pkg_search_module(VORBISFILE vorbisfile REQUIRED)
	pkg_search_module(VORBIS vorbis REQUIRED)
endif ()

add_executable(example
	smelt_test.cpp)

include_directories(
	${GLFW_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${FREETYPE_INCLUDE_DIRS}
)

if (NOT USE_CXIMAGE)
        target_link_libraries(example ${IL_LIBRARIES})
endif ()

if (NOT BUILD_DUMB)
	target_link_libraries(example ${OPENAL_LIBRARY} ${VORBISFILE_LIBRARIES} ${VORBIS_LIBRARIES})
endif ()

target_link_libraries(example
	smelt smeltext
	glfw
	${GLEW_LIBRARIES}
	${FREETYPE_LIBRARIES}
	${ZLIB_LIBRARIES}
	GL
)

get_cmake_property(__cmake_debug_var VARIABLES)
foreach (__var ${__cmake_debug_var})
	message(STATUS ">>> ${__var}=${${__var}}")
endforeach ()
