
# -------------------------------------
# Project Setup
# -------------------------------------
project(ls_game_tests CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)



# -------------------------------------
# Building and Linking Targets
# -------------------------------------
function(LS_GAME_ADD_TARGET target sources)
    add_executable(${target} ${sources} ${ARGN})
    target_link_libraries(${target} LightSky::Game LightSky::Math LightSky::Utils LightSky::Setup)
    ls_configure_cxx_target(${target})

	# Precompiled Headers
	if (LIGHTSKY_ENABLE_PCH)
		target_precompile_headers(${target} REUSE_FROM lsgame)
		target_precompile_headers(${target} REUSE_FROM lsmath)
		target_precompile_headers(${target} REUSE_FROM lsutils)
	endif()
endfunction(LS_GAME_ADD_TARGET)

LS_GAME_ADD_TARGET(lsgame_ecs_test.cpp lsgame_ecs_test.cpp)
