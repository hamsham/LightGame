
# -------------------------------------
# Project Setup
# -------------------------------------
project(lsgame_tests CXX)



# -------------------------------------
# Include Paths
# -------------------------------------
include_directories(".")



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


if(WIN32)
    set(
        LS_GAME_EXTERNAL_DEPS
        SDL2main
        SDL2
        freetype
        FreeImage
        opengl32
        mingw32
    )
else()
    set(
        LS_GAME_EXTERNAL_DEPS
        SDL2main
        SDL2
        freetype
        freeimage
        GL
    )
endif(WIN32)



# -------------------------------------
# Include Paths
# -------------------------------------
include_directories(".")



# -------------------------------------
# Source Paths
# -------------------------------------
set(LS_GAME_PROTO_COMMON_SOURCES
    "Context.cpp"
    "Display.cpp"
    "HelloPrimState.cpp"
    #"HelloTextState.cpp"
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
configure_file(testfont.ttf testfont.ttf COPYONLY)



# -------------------------------------
# Building and Linking Targets
# -------------------------------------
function(LS_GAME_ADD_TARGET trg srcs)
    add_executable(${trg} ${srcs})
    add_dependencies(${trg} ${LS_GAME_INTERNAL_DEPS})
    target_link_libraries(${trg} ${LS_GAME_INTERNAL_DEPS} ${LS_GAME_EXTERNAL_DEPS})
endfunction(LS_GAME_ADD_TARGET)

LS_GAME_ADD_TARGET(hello_lsgame "${LS_GAME_PROTO_HELLO_SOURCES}")
