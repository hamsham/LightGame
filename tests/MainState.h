
/* 
 * File:   MainState.h
 * Author: miles
 *
 * Created on February 9, 2016, 12:59 AM
 */

#ifndef MAINSTATE_H
#define MAINSTATE_H

#include "lightsky/game/Game.h"

#include "Context.h"

/*-----------------------------------------------------------------------------
 * Example System Object
-----------------------------------------------------------------------------*/
class MainState final : virtual public ls::game::GameState {
    private:
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

#endif /* MAINSTATE_H */
