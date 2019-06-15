option (USE_CXIMAGE "Use CxImage instead of DevIL" OFF)

find_package(glfw3 3.2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenAL REQUIRED)
find_package(Freetype REQUIRED)

if (NOT USE_CXIMAGE)
        find_package(DevIL REQUIRED)
endif ()

add_executable(example
	smelt_test.cpp)

include_directories(
	${GLFW_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${FREETYPE_INCLUDE_DIRS}
)

target_link_libraries(example
	glfw
	${GLEW_LIBRARIES}
	${OpenAL_LIBRARIES}
	${FREETYPE_LIBRARIES}
	smelt smeltext 
)

get_cmake_property(__cmake_debug_var VARIABLES)
foreach (__var ${__cmake_debug_var})
	message(STATUS ">>> ${__var}=${${__var}}")
endforeach ()
