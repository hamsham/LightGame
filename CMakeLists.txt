
# -------------------------------------
# Project Setup
# -------------------------------------
project(LightGame CXX)
set(OUTPUT_NAME lsgame)



# -------------------------------------
# Source Paths
# -------------------------------------
include_directories(include)

set(LS_GAME_SOURCES
    src/Dispatcher.cpp
    src/GameState.cpp
    src/GameSystem.cpp
    src/Subscriber.cpp
)

set(LS_GAME_HEADERS
    include/lightsky/game/Dispatcher.h
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
option(BUILD_SHARED_LIBS "Build Shared Libraries" OFF)

if (BUILD_SHARED_LIB)
    add_definitions(-DLS_BUILD_SHARED -DLS_SHARED)
    set(LS_LIB_TYPE SHARED)
else()
    set(LS_LIB_TYPE STATIC)
endif()

add_library(${OUTPUT_NAME} ${LS_LIB_TYPE} ${LS_GAME_SOURCES} ${LS_GAME_HEADERS})

# Dependencies
target_link_libraries(${OUTPUT_NAME} lssetup lsutils)



# -------------------------------------
# Library Installation Setup
# -------------------------------------
install(TARGETS ${OUTPUT_NAME}
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
install(DIRECTORY include/lightsky/game DESTINATION include/lightsky)
