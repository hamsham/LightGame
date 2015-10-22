/*
 * File:   game/ganeSystem.cpp
 * Author: Miles Lacey
 *
 * Created on November 15, 2013, 9:53 PM
 */

#include <new> // std::nothrow
#include <utility> // std::move
#include <chrono> // std::chrono::steady_clock, std::chrono::milliseconds

#include "lightsky/utils/Assertions.h"

#include "lightsky/game/Setup.h"
#include "lightsky/game/GameState.h"
#include "lightsky/game/GameSystem.h"

namespace ls {
namespace game {

/*-------------------------------------
    SubSystem Constructor
-------------------------------------*/
GameSystem::GameSystem() :
    tickTime{0},
    prevTime{0},
    gameList{}
{}

/*-------------------------------------
    SubSystem Move Construction
-------------------------------------*/
GameSystem::GameSystem(GameSystem&& ss) :
    tickTime{ss.tickTime},
    prevTime{ss.prevTime},
    gameList{std::move(ss.gameList)}
{}

/*-------------------------------------
    SubSystem Move Operator
-------------------------------------*/
GameSystem& GameSystem::operator=(GameSystem&& ss) {
    tickTime = ss.tickTime;
    ss.tickTime = 0;

    prevTime = ss.prevTime;
    ss.prevTime = 0;

    gameList = std::move(ss.gameList);

    return *this;
}

/*-------------------------------------
    SubSystem Destructor
-------------------------------------*/
GameSystem::~GameSystem() {
    stop();
}

/*-------------------------------------
    SubSystem Tick Time
-------------------------------------*/
void GameSystem::update_tick_time() {
    // Frame Time Management
    const uint64_t currTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();

    tickTime = currTime-prevTime;
    prevTime = currTime;
}

/*-------------------------------------
    Update all internal game states.
-------------------------------------*/
void GameSystem::update_game_states() {
    if (!gameList.size()) {
        LS_LOG_ERR("No game states are available!");
    }

    for(unsigned i = 0; i < gameList.size(); ++i) {
        GameState* const pState = gameList[i];

        switch(pState->get_state()) {
            case game_state_t::RUNNING:
                pState->on_run();
                break;
            case game_state_t::PAUSED:
                pState->on_pause();
                break;
            case game_state_t::STOPPED:
                pop_game_state(i);
                i -= 1;
                break;
            case game_state_t::STARTING:
                // if a state starts successfully, run it on the next iteration
                if (pState->on_start() == true) {
                    pState->set_state(game_state_t::RUNNING);
                }
                else {
                    // stop a state and delete it on the next iteration.
                    LS_LOG_ERR("ERROR: A new gameState was unable to start.");
                    pState->set_state(game_state_t::STOPPED);
                }
                break;
            default:
                break;
        }

    }
}

/*-------------------------------------
    SubSystem State Addition
-------------------------------------*/
bool GameSystem::push_game_state(GameState* const pState) {
    if (pState == nullptr) {
        LS_LOG_ERR("ERROR: A null pointer was pushed onto the game stack.\n");
        return false;
    }

    // states will be started in the "updateGameStates()" method.
    pState->set_parent_system(*this);
    pState->set_state(game_state_t::STARTING);
    gameList.push_back(pState);

    return true;
}

/*-------------------------------------
    SubSystem State Addition
-------------------------------------*/
void GameSystem::pop_game_state() {
    LS_DEBUG_ASSERT(gameList.size());
    pop_game_state(gameList.size()-1);
}

/*-------------------------------------
    SubSystem State Removal
-------------------------------------*/
void GameSystem::pop_game_state(GameState* const pState) {
    for (unsigned i = 0; i < gameList.size(); ++i) {
        if (gameList[i] == pState) {
            pop_game_state(i);
            break;
        }
    }
}

/*-------------------------------------
    SubSystem State Removal
-------------------------------------*/
void GameSystem::pop_game_state(unsigned index) {
    LS_DEBUG_ASSERT(index < gameList.size());

    // onStop() was moved to here in order to terminate states in a consistent manner.
    gameList[index]->on_stop();
    delete gameList[index]; // no guarantee that onStop() is in a state's destructor.
    gameList.erase(gameList.begin() + index);
}

/*-------------------------------------
    Clear SubSystem States
-------------------------------------*/
void GameSystem::clear_game_states() {
    for (GameState* const pState : gameList) {
        pState->set_state(game_state_t::STOPPED);
        delete pState;
    }

    gameList.clear();
}

/*-------------------------------------
    SubSystem State Retrieval
-------------------------------------*/
GameState const* GameSystem::get_game_state(unsigned index) const {
    LS_DEBUG_ASSERT(index < gameList.size());
    return gameList[index];
}

/*-------------------------------------
    SubSystem State Retrieval
-------------------------------------*/
GameState* GameSystem::get_game_state(unsigned index) {
    LS_DEBUG_ASSERT(index < gameList.size());
    return gameList[index];
}

/*-------------------------------------
    SubSystem State Indexing
-------------------------------------*/
unsigned GameSystem::get_game_state_index(GameState* const pState) const {
    for (unsigned i = 0; i < gameList.size(); ++i) {
        if (gameList[i] == pState) {
            return i;
        }
    }

    return UINT_MAX;
}

} // end game namespace
} // end ls namespace
