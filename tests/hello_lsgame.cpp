
#include <new> // std::nothrow
#include <iostream>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "lightsky/utils/Log.h"
#include "lightsky/math/vec2.h"
#include "lightsky/draw/Draw.h"
#include "lightsky/game/Game.h"
namespace math = ls::math;

#include "Context.h"
#include "Display.h"
#include "HelloTextState.h"
#include "HelloPrimState.h"

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

    if (!ls::draw::init_ls_draw()) {
        std::cerr << "Unable to initialize LS Draw." << std::endl;
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

    LS_LOG_GL_ERR();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    LS_LOG_GL_ERR();
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
 * Main() Methods
-----------------------------------------------------------------------------*/
/*-------------------------------------
 * Forward declarations
-------------------------------------*/
bool init_subsystems();
void terminate_subsystems();

/*-------------------------------------
 * main()
-------------------------------------*/
int main(int argc, char* argv[]) {
    ls::game::GameSystem sys{};
    
    for (int i = 0; i < argc; ++i) {
        std::cout << "Argument " << i << ": " << argv[i] << '\n';
    }
    std::cout << std::endl;

    if (!init_subsystems()) {
        std::cerr << "Unable to initialize SDL." << std::endl;
        goto quitTest;
    }
    std::cout << "LightSky Successfully initialized.\n" << std::endl;

    if (!sys.start()
    || !sys.push_game_state(new(std::nothrow) MainState{})
    //|| !sys.push_game_state(new(std::nothrow) HelloTextState{})
    || !sys.push_game_state(new(std::nothrow) HelloPrimState{})
    ) {
        std::cerr << "Unable to create the main program.\n" << std::endl;
        goto quitTest;
    }
    std::cout << "Successfully created the main program.\n" << std::endl;
    std::cout << __FILE__ << ": " << __LINE__ << std::endl;

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
