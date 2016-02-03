
/* 
 * File:   TestRenderState.h
 * Author: miles
 *
 * Created on February 2, 2016, 8:59 PM
 */

#ifndef TESTRENDERSTATE_H
#define TESTRENDERSTATE_H

#include "lightsky/draw/Camera.h"

#include "lightsky/game/GameState.h"



class TestRenderState : public ls::game::GameState {
    
    friend class ControlState;
    
    protected:
        ls::draw::Camera camera;

    public:
        virtual ~TestRenderState() = 0;

        TestRenderState();

        TestRenderState(const TestRenderState&) = delete;

        TestRenderState(TestRenderState&&);

        TestRenderState& operator=(const TestRenderState&) = delete;

        TestRenderState& operator=(TestRenderState&&);
};

#endif /* TESTRENDERSTATE_H */
