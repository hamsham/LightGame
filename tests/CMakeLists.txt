
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
if (NOT SDL2_FOUND)
    message(FATAL_ERROR "Unable to locate the library for SDL2.")
endif()

find_package(ASSIMP REQUIRED)
if (NOT assimp_FOUND)
    message(FATAL_ERROR "Unable to locate the library for ASSIMP.")
endif()

find_package(Boost REQUIRED COMPONENTS regex)
if (NOT Boost_REGEX_FOUND)
    message(FATAL_ERROR "Unable to locate the library for Boost-Regex.")
endif()



# -------------------------------------
# Include Paths
# -------------------------------------
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})

file(GLOB RECURSE LS_GAME_TEST_HEADERS "include/*.h" "include/*.hpp")



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
    ${assimp_LIBRARIES}
    ${Boost_REGEX_LIBRARY}
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
