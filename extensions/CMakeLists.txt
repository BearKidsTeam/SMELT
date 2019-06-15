find_package(Freetype REQUIRED)

file (GLOB SMELTEXT_CPP_FILES "*.cpp")

include_directories(
	${FREETYPE_INCLUDE_DIRS}
)

add_library(smeltext STATIC ${SMELTEXT_CPP_FILES})

target_link_libraries(smeltext smelt)
