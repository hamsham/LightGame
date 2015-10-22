
#include <new> // std::nothrow
#include <iostream>

#include <SDL2/SDL.h>

#include "lightsky/utils/Log.h"
#include "lightsky/math/vec2.h"
#include "lightsky/draw/Draw.h"
#include "lightsky/game/Game.h"
namespace math = ls::math;

#include "context.h"
#include "display.h"

/*-----------------------------------------------------------------------------
 * Example System Object
-----------------------------------------------------------------------------*/
class mainState final : virtual public ls::game::GameState {
    private:
        display* pDisplay = nullptr;
        context renderContext;

    public:
        mainState();

        mainState(const mainState&) = delete;

        mainState(mainState&&);

        virtual ~mainState();

        mainState& operator=(const mainState&) = delete;

        mainState& operator=(mainState&&);

    protected:
        virtual bool onStart() override;

        virtual void onRun() override;

        virtual void onStop() override;

    private:
        void onKeyboardUpEvent(const SDL_KeyboardEvent& e);

        void onWindowEvent(const SDL_WindowEvent& e);
};

/*-------------------------------------
 * Destructor
-------------------------------------*/
mainState::~mainState() {
}

/*-------------------------------------
 * Contstructor
-------------------------------------*/
mainState::mainState() {
}

/*-------------------------------------
 * Move Constructor
-------------------------------------*/
mainState::mainState(mainState&& ms) :
    GameState{std::move(ms)}
{}

/*-------------------------------------
 * Move Operator
-------------------------------------*/
mainState& mainState::operator=(mainState&& ms) {
    GameState::operator=(std::move(ms));

    return *this;
}

/*-------------------------------------
 * System Startup
-------------------------------------*/
bool mainState::onStart() {
    pDisplay = new(std::nothrow) display{};
    if (!pDisplay || !pDisplay->init(math::vec2i{800, 600})) {
        std::cerr << "Unable to create a display." << std::endl;
        return false;
    }

    pDisplay->setFullScreenMode(FULLSCREEN_WINDOW);

    if (!renderContext.init(*pDisplay)) {
        std::cerr << "Unable to create a render context." << std::endl;
        return false;
    }

    return true;
}

/*-------------------------------------
 * System Runtime
-------------------------------------*/
void mainState::onRun() {
    renderContext.makeCurrent(*pDisplay);
    renderContext.flip(*pDisplay);

    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_WINDOWEVENT:       this->onWindowEvent(e.window);          break;
            case SDL_KEYUP:             this->onKeyboardUpEvent(e.key);         break;
            default: break;
        }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/*-------------------------------------
 * Keyboard Event
-------------------------------------*/
void mainState::onKeyboardUpEvent(const SDL_KeyboardEvent& e) {
    const SDL_Keycode key = e.keysym.scancode;

    if (key == SDL_SCANCODE_ESCAPE) {
        ls::game::GameSystem& parentSys = getParentSystem();
        for (unsigned i = 0; i < parentSys.getNumGameStates(); ++i) {
            ls::game::GameState* const pState = parentSys.getGameState(i);
            pState->setStateStatus(ls::game::game_state_t::STOPPED);
        }
    }
}

/*-------------------------------------
 * Window Event
-------------------------------------*/
void mainState::onWindowEvent(const SDL_WindowEvent& e) {
    if (e.event == SDL_WINDOWEVENT_CLOSE) {
        getParentSystem().stop();
    }
}

/*-------------------------------------
 * System Stop
-------------------------------------*/
void mainState::onStop() {
    renderContext.terminate();
    delete pDisplay;
    pDisplay = nullptr;
}

/*-----------------------------------------------------------------------------
 * Main() Methids
-----------------------------------------------------------------------------*/
/*-------------------------------------
 * Forward declarations
-------------------------------------*/
bool initSubsystems();
void terminateSubsystems();

/*-------------------------------------
 * main()
-------------------------------------*/
int main() {
    ls::game::GameSystem sys{};

    if (!initSubsystems()) {
        std::cerr << "Unable to initialize SDL." << std::endl;
        goto quitTest;
    }
    std::cout << "LightSky Successfully initialized.\n" << std::endl;

    if (!sys.start() || !sys.pushGameState(new(std::nothrow) mainState{})) {
        std::cerr << "Unable to create the main program.\n" << std::endl;
        goto quitTest;
    }
    std::cout << "Successfully created the main program.\n" << std::endl;

    while (sys.isRunnable()) {
        sys.run();
    }

    quitTest:
    sys.stop();
    terminateSubsystems();
    std::cout << "LightSky successfully terminated.\n" << std::endl;

    return 0;
}

/*-------------------------------------
    initialization.
-------------------------------------*/
bool initSubsystems() {
    if (SDL_WasInit(0) == SDL_INIT_EVERYTHING) {
        return true;
    }

    SDL_SetMainReady();

    constexpr uint32_t sdlInitFlags = 0
        | SDL_INIT_TIMER
        | SDL_INIT_AUDIO
        | SDL_INIT_VIDEO
        | SDL_INIT_EVENTS
        | SDL_INIT_JOYSTICK
        | SDL_INIT_GAMECONTROLLER
        | 0;

    if (SDL_Init(sdlInitFlags) < 0) {
        LS_LOG_ERR(
            "Unable to initialize SDL due to error ", SDL_GetError(), '\n',
            "Complain to your local programmer.\n"
        );
        return false;
    }
    LS_LOG_MSG(
        "Successfully initialized SDL.\n",
        SDL_GetError(), '\n'
    );

    /*
     * Setup the necessary parameters for OpenGL 3.3
     */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG
#ifdef LS_DEBUG
        | SDL_GL_CONTEXT_DEBUG_FLAG
#endif
    );
    SDL_ClearError();

    return true;
}

/*-------------------------------------
    termination
-------------------------------------*/
void terminateSubsystems() {
    if (SDL_WasInit(0)) {
        SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
        SDL_Quit();
    }
}
