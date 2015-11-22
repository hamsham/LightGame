
# -------------------------------------
# Project Setup
# -------------------------------------
project(LightGame CXX)
set(OUTPUT_NAME lsgame)



# -------------------------------------
# Source Paths
# -------------------------------------
include_directories("include")

set(LS_GAME_SOURCES
    "src/game/Dispatcher.cpp"
    "src/game/GameState.cpp"
    "src/game/GameSystem.cpp"
    "src/game/Subscriber.cpp"
)



# -------------------------------------
# Library Setup
# -------------------------------------
add_library("${OUTPUT_NAME}" STATIC "${LS_GAME_SOURCES}")

# Dependencies
target_link_libraries("${OUTPUT_NAME}" lssetup lsutils lsmath)



# -------------------------------------
# Library Installation Setup
# -------------------------------------
install(TARGETS "${OUTPUT_NAME}" ARCHIVE DESTINATION lib)
install(DIRECTORY include/lightsky/game DESTINATION include/lightsky)



# -------------------------------------
# LS Game Testing Area
# -------------------------------------
set(LS_BUILD_GAME_TESTS TRUE CACHE BOOL "Build tests for the LightGame library.")

if(LS_BUILD_GAME_TESTS)
    add_subdirectory("tests")
endif(LS_BUILD_GAME_TESTS)
