/*
 * File:   game/gameState.cpp
 * Author: Miles Lacey
 *
 * Created on November 15, 2013, 8:45 PM
 */

#include "lightsky/game/GameState.h"

namespace ls {
namespace game {

/*-------------------------------------
    Destructor
-------------------------------------*/
GameState::~GameState() {
}

/*-------------------------------------
    Constructor
-------------------------------------*/
GameState::GameState() {
}

/*-------------------------------------
    Move Constructor
-------------------------------------*/
GameState::GameState(GameState&& gs) :
    currentState{gs.currentState},
    pSystem{gs.pSystem}
{
    gs.currentState = game_state_t::STOPPED;
}

/*-------------------------------------
    Move Operator
-------------------------------------*/
GameState& GameState::operator=(GameState&& gs) {

    currentState = gs.currentState;
    gs.currentState = game_state_t::STOPPED;

    pSystem = gs.pSystem;

    return *this;
}

/*-------------------------------------
    Parent Sub-System Assignment.
-------------------------------------*/
void GameState::setParentSystem(GameSystem& pSys) {
    pSystem = &pSys;
}

/*-------------------------------------
    Start the current system's running state.
-------------------------------------*/
bool GameState::onStart() {
    return true;
}

/*-------------------------------------
    Engage the current system's running state.
-------------------------------------*/
void GameState::onRun() {
}

/*-------------------------------------
    Pause the current system's running state.
-------------------------------------*/
void GameState::onPause() {
}

/*-------------------------------------
    Stop the current system's running state.
-------------------------------------*/
void GameState::onStop() {
}

} // end game namespace
} // end ls namespace
