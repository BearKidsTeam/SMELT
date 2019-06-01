find_package(Freetype REQUIRED)

file (GLOB SMELTTEXT_CPP_FILES "*.cpp")

include_directories(
	${FREETYPE_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_library(smelttext STATIC ${SMELTTEXT_CPP_FILES})
