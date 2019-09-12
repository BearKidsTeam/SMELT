find_package(Freetype REQUIRED)

file (GLOB SMELTEXT_CPP_FILES "*.cpp")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${FREETYPE_INCLUDE_DIRS})

add_library(smeltext STATIC ${SMELTEXT_CPP_FILES})
