/* 
 * File:   ControlState.h
 * Author: miles
 *
 * Created on August 5, 2014, 9:50 PM
 */

#ifndef CONTROLSTATE_H
#define	CONTROLSTATE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>

#include "lightsky/game/GameState.h"

class TestRenderState;

/**----------------------------------------------------------------------------
 * Global Controller testing state
-----------------------------------------------------------------------------*/
class ControlState final : virtual public ls::game::GameState {
        friend class HelloPrimState;
        friend class HelloTextState;

        /*
         * Event Management
         */
    private:
        int mouseX = 0;
        int mouseY = 0;
        bool* pKeyStates = nullptr;
        TestRenderState* pRenderState = nullptr;
        // ^set by the state that instantiated *this

        void set_render_state       (TestRenderState * const);

        void on_window_event        (const SDL_WindowEvent&);
        void on_key_up_event        (const SDL_KeyboardEvent&);
        void on_key_down_event      (const SDL_KeyboardEvent&);
        void on_mouse_move_event    (const SDL_MouseMotionEvent&);
        void on_mouse_down_event    (const SDL_MouseButtonEvent&);
        void on_wheel_event         (const SDL_MouseWheelEvent&);

    public:
        virtual ~ControlState       ();

        ControlState                ();
        ControlState                (const ControlState&) = delete;
        ControlState                (ControlState&&);

        ControlState& operator=     (const ControlState&) = delete;
        ControlState& operator=     (ControlState&&);

        bool on_start               () override;
        void on_run                 () override;
        void on_stop                () override;
};

#endif	/* CONTROLSTATE_H */

