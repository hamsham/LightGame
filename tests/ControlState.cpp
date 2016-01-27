/* 
 * File:   ControlState.cpp
 * Author: miles
 * 
 * Created on August 5, 2014, 9:50 PM
 */

#include <new> // std::nothrow
#include <utility> // std::move

#include "lightsky/math/Math.h"

#include "lightsky/game/GameSystem.h"

#include "ControlState.h"
#include "HelloTextState.h"

namespace math = ls::math;

namespace {
enum {
    TEST_MAX_KEYBORD_STATES = 282, // according to https://wiki.libsdl.org/SDLScancodeLookup
};
} // end anonymous namespace

/*-------------------------------------
 * Construction, Assignment, and Destruction
-------------------------------------*/
ControlState::~ControlState() {
}

ControlState::ControlState() :
    GameState{}
{
    SDL_StopTextInput();
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

ControlState::ControlState(ControlState&& state) :
    GameState{std::move(state)},
    mouseX{state.mouseX},
    mouseY{state.mouseY},
    pKeyStates{state.pKeyStates}
{
    state.mouseX = 0;
    state.mouseY = 0;
    state.pKeyStates = nullptr;
    
    SDL_StopTextInput();
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

ControlState& ControlState::operator=(ControlState&& state) {
    GameState::operator=(std::move(state));
    
    mouseX = state.mouseX;
    state.mouseX = 0;
    
    mouseY = state.mouseY;
    state.mouseY = 0;
    
    pKeyStates = state.pKeyStates;
    state.pKeyStates = nullptr;
    
    return *this;
}

/*-------------------------------------
 * Parent renderer state management
-------------------------------------*/
void ControlState::set_render_state(HelloTextState* const pState) {
    pRenderState = pState;
}

/*-------------------------------------
 * Starting state
-------------------------------------*/
bool ControlState::on_start() {
    pKeyStates = new(std::nothrow) bool[TEST_MAX_KEYBORD_STATES];
    
    if (pKeyStates == nullptr) {
        return false;
    }
    
    // initialize the keybord
    for (unsigned i = 0;  i < TEST_MAX_KEYBORD_STATES; ++i) {
        pKeyStates[i] = false;
    }
    
    return true;
}

/*-------------------------------------
 * Running state
-------------------------------------*/
void ControlState::on_run() {
    const math::vec2i&& displayRes{800, 600};
    glViewport(0, 0, displayRes[0], displayRes[1]);
    
    SDL_Event e;
    
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_WINDOWEVENT:       this->on_window_event(e.window);        break;
            case SDL_KEYUP:             this->on_key_up_event(e.key);           break;
            case SDL_KEYDOWN:           this->on_key_down_event(e.key);         break;
            case SDL_MOUSEMOTION:       this->on_mouse_move_event(e.motion);    break;
            case SDL_MOUSEBUTTONDOWN:   this->on_mouse_down_event(e.button);    break;
            case SDL_MOUSEWHEEL:        this->on_wheel_event(e.wheel);          break;
            default: break;
        }
    }
    
    // update the camera position
    ls::draw::Camera& mainCam = pRenderState->camera;
    math::vec3 pos = {0.f};
    const float moveSpeed = 0.05f * (float)get_parent_system().get_tick_time();
    
    if (pKeyStates[SDL_SCANCODE_W]) {
        pos[2] += moveSpeed;
    }
    if (pKeyStates[SDL_SCANCODE_S]) {
        pos[2] -= moveSpeed;
    }
    if (pKeyStates[SDL_SCANCODE_A]) {
        pos[0] += moveSpeed;
    }
    if (pKeyStates[SDL_SCANCODE_D]) {
        pos[0] -= moveSpeed;
    }
    if (pKeyStates[SDL_SCANCODE_Q]) {
        pos[1] += moveSpeed;
    }
    if (pKeyStates[SDL_SCANCODE_E]) {
        pos[1] -= moveSpeed;
    }
    if (pKeyStates[SDL_SCANCODE_1]) {
        mainCam.set_view_mode(ls::draw::camera_mode_t::ARCBALL);
    }
    if (pKeyStates[SDL_SCANCODE_2]) {
        mainCam.set_view_mode(ls::draw::camera_mode_t::FIRST_PERSON);
    }
    
    mainCam.move(pos);
}

/*-------------------------------------
 * Stopping state
-------------------------------------*/
void ControlState::on_stop() {
    mouseX = 0;
    mouseY = 0;
    
    delete [] pKeyStates;
    pKeyStates = nullptr;
}

/*-------------------------------------
 * Key Up Event
-------------------------------------*/
void ControlState::on_key_up_event(const SDL_KeyboardEvent& e) {
    const SDL_Keycode key = e.keysym.scancode;
    
    pKeyStates[key] = false;
}

/*-------------------------------------
 * Key Down Event
-------------------------------------*/
void ControlState::on_key_down_event(const SDL_KeyboardEvent& e) {
    const SDL_Keycode key = e.keysym.scancode;
    
    pKeyStates[key] = true;
}

/*-------------------------------------
 * Window Event
-------------------------------------*/
void ControlState::on_window_event(const SDL_WindowEvent& e) {
    if (e.event == SDL_WINDOWEVENT_CLOSE) {
        get_parent_system().stop();
    }
}

/*-------------------------------------
 * Mouse Button Down Event
-------------------------------------*/
void ControlState::on_mouse_down_event(const SDL_MouseButtonEvent& e) {
    // Allow the mouse to enter/exit the window when the user pleases.
    if (e.button == SDL_BUTTON_LEFT) {
        // testing mouse capture for framebuffer/window resizing
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    else if (e.button == SDL_BUTTON_RIGHT) {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }
}

/*-------------------------------------
 * Mouse Button Down Event
-------------------------------------*/
void ControlState::on_mouse_move_event(const SDL_MouseMotionEvent& e) {
    // Prevent the orientation from drifting by keeping track of the relative mouse offset
    if (this->get_state() == ls::game::game_state_t::PAUSED
    || SDL_GetRelativeMouseMode() == SDL_FALSE
    || (mouseX == e.xrel && mouseY == e.yrel)
    ) {
        // I would rather quit the function than have unnecessary LERPs and
        // quaternion multiplications.
        return;
    }
    
    mouseX = e.xrel;
    mouseY = e.yrel;
    
    // Get the current mouse position and LERP from the previous mouse position.
    // The mouse position is divided by the window's resolution in order to normalize
    // the mouse delta between 0 and 1. This allows for the camera's orientation to
    // be LERPed without the need for multiplying it by the last time delta.
    // As a result, the camera's movement becomes as smooth and natural as possible.
    const math::vec2&& fRes{800.f, 600.f};
    const math::vec3&& mouseDelta = math::vec3{
        (float)mouseX/fRes[0], (float)mouseY/fRes[1], 0.f
    };
    
    pRenderState->camera.rotate(mouseDelta);
}

/*-------------------------------------
 * Mouse Button Down Event
-------------------------------------*/
void ControlState::on_wheel_event(const SDL_MouseWheelEvent& e) {
    constexpr float totalAngles = -1.f / 120.f;
    const float horizAngles = (float)e.x;
    const float vertAngles = (float)e.y;
    
    ls::draw::Camera& cam = pRenderState->camera;
    cam.rotate(math::vec3{horizAngles, 0.f, vertAngles * totalAngles});
}