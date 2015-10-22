
#include <new> // std::nothrow
#include <iostream>

#include <SDL2/SDL.h>

#include "lightsky/utils/Log.h"
#include "lightsky/math/vec2.h"
#include "lightsky/draw/Draw.h"
#include "lightsky/game/Game.h"
namespace math = ls::math;

#include "Context.h"
#include "Display.h"

/*-----------------------------------------------------------------------------
 * Example System Object
-----------------------------------------------------------------------------*/
class MainState final : virtual public ls::game::GameState {
    private:
        Display* pDisplay = nullptr;
        Context renderContext;

    public:
        MainState();

        MainState(const MainState&) = delete;

        MainState(MainState&&);

        virtual ~MainState();

        MainState& operator=(const MainState&) = delete;

        MainState& operator=(MainState&&);

    protected:
        virtual bool on_start() override;

        virtual void on_run() override;

        virtual void on_stop() override;

    private:
        void on_key_up_event(const SDL_KeyboardEvent& e);

        void on_window_event(const SDL_WindowEvent& e);
};

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
    pDisplay = new(std::nothrow) Display{};
    if (!pDisplay || !pDisplay->init(math::vec2i{800, 600})) {
        std::cerr << "Unable to create a display." << std::endl;
        return false;
    }

    pDisplay->set_fullscreen_mode(FULLSCREEN_WINDOW);

    if (!renderContext.init(*pDisplay)) {
        std::cerr << "Unable to create a render context." << std::endl;
        return false;
    }

    return true;
}

/*-------------------------------------
 * System Runtime
-------------------------------------*/
void MainState::on_run() {
    renderContext.make_current(*pDisplay);
    renderContext.flip(*pDisplay);

    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_WINDOWEVENT:
                this->on_window_event(e.window);
                break;

            case SDL_KEYUP:
                this->on_key_up_event(e.key);
                break;

            default:
                break;
        }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/*-------------------------------------
 * Keyboard Event
-------------------------------------*/
void MainState::on_key_up_event(const SDL_KeyboardEvent& e) {
    const SDL_Keycode key = e.keysym.scancode;

    if (key == SDL_SCANCODE_ESCAPE) {
        ls::game::GameSystem& parentSys = get_parent_system();
        for (unsigned i = 0; i < parentSys.get_num_game_states(); ++i) {
            ls::game::GameState* const pState = parentSys.get_game_state(i);
            pState->set_state(ls::game::game_state_t::STOPPED);
        }
    }
}

/*-------------------------------------
 * Window Event
-------------------------------------*/
void MainState::on_window_event(const SDL_WindowEvent& e) {
    if (e.event == SDL_WINDOWEVENT_CLOSE) {
        get_parent_system().stop();
    }
}

/*-------------------------------------
 * System Stop
-------------------------------------*/
void MainState::on_stop() {
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
bool init_subsystems();
void terminate_subsystems();

/*-------------------------------------
 * main()
-------------------------------------*/
int main() {
    ls::game::GameSystem sys{};

    if (!init_subsystems()) {
        std::cerr << "Unable to initialize SDL." << std::endl;
        goto quitTest;
    }
    std::cout << "LightSky Successfully initialized.\n" << std::endl;

    if (!sys.start() || !sys.push_game_state(new(std::nothrow) MainState{})) {
        std::cerr << "Unable to create the main program.\n" << std::endl;
        goto quitTest;
    }
    std::cout << "Successfully created the main program.\n" << std::endl;

    while (sys.is_runnable()) {
        sys.run();
    }

    quitTest:
    sys.stop();
    terminate_subsystems();
    std::cout << "LightSky successfully terminated.\n" << std::endl;

    return 0;
}

/*-------------------------------------
    initialization.
-------------------------------------*/
bool init_subsystems() {
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
void terminate_subsystems() {
    if (SDL_WasInit(0)) {
        SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
        SDL_Quit();
    }
}
