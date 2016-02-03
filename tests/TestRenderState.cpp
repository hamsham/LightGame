
/* 
 * File:   TestRenderState.cpp
 * Author: miles
 * 
 * Created on February 2, 2016, 8:59 PM
 */

#include <utility>

#include "TestRenderState.h"

/*-------------------------------------
-------------------------------------*/
TestRenderState::~TestRenderState() {
}

/*-------------------------------------
-------------------------------------*/
TestRenderState::TestRenderState() :
    GameState{}
{}

/*-------------------------------------
-------------------------------------*/
TestRenderState::TestRenderState(TestRenderState&& rs) :
    GameState{std::move(rs)}
{}

/*-------------------------------------
-------------------------------------*/
TestRenderState& TestRenderState::operator =(TestRenderState&& rs) {
    GameState::operator=(std::move(rs));
    return *this;
}
