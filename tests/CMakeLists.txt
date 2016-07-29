
# -------------------------------------
# Project Setup
# -------------------------------------
project(ls_game_tests CXX)



# -------------------------------------
# CMake Setup
# -------------------------------------
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules" ${CMAKE_MODULE_PATH})



# -------------------------------------
# Finders
# -------------------------------------
# Only using OpenGL for its libraries, not headers
include(FindOpenGL)

include(FindFreetype)

find_package(SDL2 REQUIRED)
find_package(ASSIMP REQUIRED)



# -------------------------------------
# Include Paths
# -------------------------------------
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})

file(GLOB RECURSE __LS_GAME_TEST_H__ EADERS "include/*.h" "include/*.hpp")



# -------------------------------------
# Dependency Setup
# -------------------------------------
set(
    LS_GAME_INTERNAL_DEPS
    ls_setup
    ls_utils
    ls_math
    ls_draw
    ls_game
)

set(
    LS_GAME_EXTERNAL_DEPS
    ${OPENGL_gl_LIBRARY}
    ${SDL2_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${ASSIMP_LIBRARIES}
    freeimage
)


# -------------------------------------
# Source Paths
# -------------------------------------
set(LS_GAME_TEST_SOURCES
    "Context.cpp"
    "ControlState.cpp"
    "Display.cpp"
    "HelloMeshState.cpp"
    "HelloPropertyState.cpp"
    "HelloPrimState.cpp"
    "HelloTextState.cpp"
    "MainState.cpp"
)

set(
    LS_GAME_TEST_SOURCES_HELLOWORLD
    "main.cpp"
    ${LS_GAME_TEST_SOURCES}
    ${LS_GAME_TEST_HEADERS}
)



# -------------------------------------
# Misc Files
# -------------------------------------
file(COPY "${PROJECT_SOURCE_DIR}/testdata" DESTINATION "${PROJECT_BINARY_DIR}")



# -------------------------------------
# Building and Linking Targets
# -------------------------------------
function(LS_GAME_ADD_TARGET trg srcs)
    add_executable(${trg} ${srcs})
    add_dependencies(${trg} ${LS_GAME_INTERNAL_DEPS})
    target_link_libraries(${trg} ${LS_GAME_INTERNAL_DEPS} ${LS_GAME_EXTERNAL_DEPS})
endfunction(LS_GAME_ADD_TARGET)

LS_GAME_ADD_TARGET(hello_eds_game "${LS_GAME_TEST_SOURCES_HELLOWORLD}")
