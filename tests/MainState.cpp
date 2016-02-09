
/* 
 * File:   MainState.cpp
 * Author: miles
 * 
 * Created on February 9, 2016, 12:59 AM
 */

#include "lightsky/utils/Log.h"
#include "lightsky/draw/Draw.h"

#include "main.h"
#include "MainState.h"
#include "Display.h"

namespace math = ls::math;

Display* global::pDisplay = nullptr;

/*-------------------------------------
 * Destructor
-------------------------------------*/
MainState::~MainState() {
}

/*-------------------------------------
 * Contstructor
-------------------------------------*/
MainState::MainState() {
}

/*-------------------------------------
 * Move Constructor
-------------------------------------*/
MainState::MainState(MainState&& ms) :
    GameState{std::move(ms)}
{}

/*-------------------------------------
 * Move Operator
-------------------------------------*/
MainState& MainState::operator=(MainState&& ms) {
    GameState::operator=(std::move(ms));

    return *this;
}

/*-------------------------------------
 * System Startup
-------------------------------------*/
bool MainState::on_start() {
    constexpr math::vec2i winSize = {get_test_window_width(), get_test_window_height()};
    
    global::pDisplay = new(std::nothrow) Display{};
    if (!global::pDisplay
    || !global::pDisplay->init(winSize)
    ) {
        std::cerr << "Unable to create a display." << std::endl;
        return false;
    }

    if (!renderContext.init(*global::pDisplay)) {
        std::cerr << "Unable to create a render context." << std::endl;
        return false;
    }
    LS_LOG_GL_ERR();

    if (!ls::draw::init_ls_draw()) {
        std::cerr << "Unable to initialize LS Draw." << std::endl;
        return false;
    }
    LS_LOG_GL_ERR();
    
    glClearColor(0.5f, 0.5f, 0.65f, 1.f);

    return true;
}

/*-------------------------------------
 * System Runtime
-------------------------------------*/
void MainState::on_run() {
    LS_LOG_GL_ERR();
    
    renderContext.make_current(*global::pDisplay);
    LS_LOG_GL_ERR();
    
    renderContext.flip(*global::pDisplay);
    LS_LOG_GL_ERR();
    
    const math::vec2i&& displayRes = global::pDisplay->get_resolution();
    glViewport(0, 0, displayRes[0], displayRes[1]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    LS_LOG_GL_ERR();
}

/*-------------------------------------
 * System Stop
-------------------------------------*/
void MainState::on_stop() {
    renderContext.terminate();
    delete global::pDisplay;
    global::pDisplay = nullptr;
}
