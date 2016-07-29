
/*
 * File:   MainState.h
 * Author: miles
 *
 * Created on February 9, 2016, 12:59 AM
 */

#ifndef MAINSTATE_H
#define MAINSTATE_H

#include "ls/utils/Pointer.h"
#include "ls/game/Game.h"

#include "Context.h"



/*-----------------------------------------------------------------------------
 * Forward Declarations
-----------------------------------------------------------------------------*/
class Display;



/*-----------------------------------------------------------------------------
 * Objects global to the main program state
-----------------------------------------------------------------------------*/
namespace global {
    extern ls::utils::Pointer<Display> pDisplay;
}



/*-----------------------------------------------------------------------------
 * Example System Object
-----------------------------------------------------------------------------*/
class MainState final : virtual public ls::game::GameState {
  private:
    Context renderContext;
    
    bool bootstrap_subsystems();
    
    bool setup_substates();

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

#endif /* MAINSTATE_H */
