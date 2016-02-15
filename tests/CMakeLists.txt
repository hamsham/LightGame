
# -------------------------------------
# Project Setup
# -------------------------------------
project(lsgame_tests CXX)



# -------------------------------------
# CMake Setup
# -------------------------------------
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules" ${CMAKE_MODULE_PATH})



# -------------------------------------
# Finders
# -------------------------------------
include(FindOpenGL)
include(FindFreeType)

find_package(SDL2 REQUIRED)



# -------------------------------------
# Include Paths
# -------------------------------------
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})



# -------------------------------------
# Definitions
# -------------------------------------
if (CMAKE_BUILD_TYPE MATCHES "DEBUG" OR CMAKE_BUILD_TYPE MATCHES "RELWITHDEBINFO")
    message("Building LightSky tests with debug info.")
    add_definitions(-DLS_DEBUG)
endif()



# -------------------------------------
# Dependency Setup
# -------------------------------------
set(
    LS_GAME_INTERNAL_DEPS
    lssetup
    lsutils
    lsmath
    lsdraw
    lsgame
)

set(
    LS_GAME_EXTERNAL_DEPS
    ${OPENGL_gl_LIBRARY}
    ${SDL2_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    FreeImage
)



# -------------------------------------
# Source Paths
# -------------------------------------
set(LS_GAME_PROTO_COMMON_SOURCES
    "Context.cpp"
    "Display.cpp"
    "MainState.cpp"
    "TestRenderState.cpp"
    "HelloPrimState.cpp"
    "HelloTextState.cpp"
    "ControlState.cpp"
    "TextGeometryLoader.cpp"
)

set(
    LS_GAME_PROTO_HELLO_SOURCES
    "hello_lsgame.cpp"
    ${LS_GAME_PROTO_COMMON_SOURCES}
)



# -------------------------------------
# Misc Files
# -------------------------------------
configure_file(testfont.ttf ${CMAKE_CURRENT_BINARY_DIR}/testfont.ttf COPYONLY)
configure_file(lorem_ipsum.txt ${CMAKE_CURRENT_BINARY_DIR}/lorem_ipsum.txt COPYONLY)



# -------------------------------------
# Building and Linking Targets
# -------------------------------------
function(LS_GAME_ADD_TARGET trg srcs)
    add_executable(${trg} ${srcs})
    add_dependencies(${trg} ${LS_GAME_INTERNAL_DEPS})
    target_link_libraries(${trg} ${LS_GAME_INTERNAL_DEPS} ${LS_GAME_EXTERNAL_DEPS})
endfunction(LS_GAME_ADD_TARGET)

LS_GAME_ADD_TARGET(hello_lsgame "${LS_GAME_PROTO_HELLO_SOURCES}")
