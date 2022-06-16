
# -------------------------------------
# Project Setup
# -------------------------------------
project(LightGame CXX)
set(OUTPUT_NAME lsgame)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)



# -------------------------------------
# Source Paths
# -------------------------------------
set(LS_GAME_SOURCES
    src/Component.cpp
    src/Dispatcher.cpp
    src/ECSDatabase.cpp
    src/GameState.cpp
    src/GameSystem.cpp
    src/Subscriber.cpp
)

set(LS_GAME_HEADERS
    include/lightsky/game/Component.hpp
    include/lightsky/game/Dispatcher.h
    include/lightsky/game/ECSDatabase.hpp
    include/lightsky/game/Entity.hpp
    include/lightsky/game/Event.h
    include/lightsky/game/Game.h
    include/lightsky/game/GameState.h
    include/lightsky/game/GameSystem.h
    include/lightsky/game/Manager.h
    include/lightsky/game/Subscriber.h
)



# -------------------------------------
# Library Setup
# -------------------------------------
add_library(${OUTPUT_NAME} ${LS_GAME_SOURCES} ${LS_GAME_HEADERS})

ls_configure_target(${OUTPUT_NAME})
target_include_directories(${OUTPUT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
target_link_libraries(${OUTPUT_NAME} LightSky::Utils LightSky::Setup)



# -------------------------------------
# Library Installation Setup
# -------------------------------------
install(TARGETS ${OUTPUT_NAME}
    EXPORT LightGame
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
install(DIRECTORY include/lightsky/game DESTINATION include/lightsky)

install(EXPORT LightGame
    FILE LightGame.cmake
    NAMESPACE LightSky::
    DESTINATION lib/cmake/LightSky
)

export(EXPORT LightGame NAMESPACE LightSky::)
add_library(LightSky::Game ALIAS lsgame)



# -------------------------------------
# Precompiled Headers
# -------------------------------------
if (LIGHTSKY_ENABLE_PCH)
    target_precompile_headers(
        lsgame
        PUBLIC
            ${LS_GAME_HEADERS}
    )
endif()



# -------------------------------------
# Unit Tests
# -------------------------------------
option(LS_GAME_BUILD_TESTS "Build tests for the LightGame library." ON)

if(LS_GAME_BUILD_TESTS)
    add_subdirectory(tests)
endif()
