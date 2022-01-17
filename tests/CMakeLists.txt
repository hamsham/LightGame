
# -------------------------------------
# Project Setup
# -------------------------------------
project(ls_game_tests CXX)



# -------------------------------------
# Building and Linking Targets
# -------------------------------------
function(LS_GAME_ADD_TARGET target sources)
    add_executable(${target} ${sources} ${ARGN})
    target_link_libraries(${target} LightSky::Game LightSky::Math LightSky::Utils LightSky::Setup)
    ls_configure_target(${target})

	# Precompiled Headers
	if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0")
		target_precompile_headers(${target} REUSE_FROM lsgame)
		target_precompile_headers(${target} REUSE_FROM lsmath)
		target_precompile_headers(${target} REUSE_FROM lsutils)
	endif()
endfunction(LS_GAME_ADD_TARGET)

LS_GAME_ADD_TARGET(lsgame_ecs_test.cpp lsgame_ecs_test.cpp)
