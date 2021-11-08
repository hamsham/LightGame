
# -------------------------------------
# Project Setup
# -------------------------------------
project(ls_math_tests CXX)



# -------------------------------------
# Building and Linking Targets
# -------------------------------------
function(LS_GAME_ADD_TARGET target sources)
    add_executable(${target} ${sources} ${ARGN})
    target_link_libraries(${target} LightSky::Game LightSky::Math LightSky::Utils LightSky::Setup)
    ls_configure_target(${target})
endfunction(LS_GAME_ADD_TARGET)

LS_GAME_ADD_TARGET(lsgame_ecs_test.cpp lsgame_ecs_test.cpp)
