/*
 * File:   game/gameState.h
 * Author: Miles Lacey
 *
 * Created on November 15, 2013, 8:45 PM
 */

#ifndef LS_GAME_GAME_STATE_H
#define LS_GAME_GAME_STATE_H

#include <cstdint>

#include "lightsky/setup/Api.h"



namespace ls
{
namespace game
{



class GameSystem;



/**
 * A game_state_t is a type used in order to help manage interactions between
 * ls::game::system objects and different game state objects.
*/
enum class game_state_status_t
{
    PAUSED = 0,
    RUNNING,
    STOPPING,
    STOPPED,
    STARTING
};



/**
 * @brief Game State Base Class
 *
 * An Abstract base class that can be overridden to organize games onto stacks
 * within an lsSubsystem. gameState objects are pushed onto a "stack" managed
 * by the lsSubsystem. Game states are passed hardware events by subsystems
 * when they are running.
*/
class GameState
{
    friend class GameSystem;

  private:
    /**
     * The current state that is used by *this. This variable is also
     * assigned by the parent subsystem and should not be modified.
     */
    game_state_status_t currentState = game_state_status_t::STOPPED;

    /**
     * A "const" pointer to the parent lsSubsystem which manages *this.
     * this pointer is assigned automatically when a system is pushed onto
     * the subsystem's stack. Do not try to modify this.
     */
    mutable GameSystem* pSystem; // set by an lsSubsystem upon initialization

    /**
     * Used by the parent subsystem to help with gameState management.
     *
     * @param sys
     * A reference to the parent subsystem.
     */
    void set_parent_system(GameSystem& sys);

  protected:
    /*---------------------------------------------------------------------
        System Events
    ---------------------------------------------------------------------*/
    /**
     * The onStart method is called by the parent subsystem when *this
     * object has been notified to start. Place all memory allocations here.
     *
     * @return bool
     * TRUE to indicate that *this has successfully initialized, FALSE if
     * otherwise.
     */
    virtual bool on_start();

    /**
     * The onStop method is used by the parent subsystem to indicate that
     * *this game state should terminate. Place all memory cleanup here.
     */
    virtual void on_stop();

    /**
     * This method is used to tell the current game state that it should
     * update its child components/variables/state.
     */
    virtual void on_run();

    /**
     * This method is used by the parent subsystem to tell *this game state
     * that it's paused.
     */
    virtual void on_pause();

  public:
    /**
     * @brief Destructor
     *
     * It is recommended to perform all object cleanup using the "onStop()"
     * method. This should be used as a last-resort.
     */
    virtual ~GameState() = 0;

    /**
     * @brief Default Constructor
     */
    GameState();

    /**
     * @brief Copy Constructor -- DELETED
     *
     * LightSky makes no attempts to copy game state objects.
     */
    GameState(const GameState&) = delete;

    /**
     * @brief Move Constructor
     *
     * Moves all memory allocations from the input parameter into *this.
     * This method may be called at some point by the parent subsystem.
     *
     * @param gs
     * An r-value reference to a game state who's members are to be moved
     * into *this.
     */
    GameState(GameState&& gs);

    /**
     * @brief Copy Operator -- DELETED
     *
     * LightSky makes no attempts to copy game state objects.
     */
    GameState& operator=(const GameState&) = delete;

    /**
     * @brief Move Operator
     *
     * Moves all memory allocations from the input parameter into *this.
     * This method may be called at some point by the parent subsystem.
     *
     * @param gs
     * An r-value reference to a game state that's about to go out
     * of scope and be terminated.
     *
     * @return A reference to *this.
     */
    GameState& operator=(GameState&& gs);

    /**
     * @brief Get the current state of *this object.
     *
     * This object's state is used by the parent subsystem to determine when
     * it should go out of scope and terminate.
     *
     * @return game_state_t
     */
    game_state_status_t get_state() const;

    /**
     * @brief Set the operational state of *this.
     *
     * @param s
     * A game_state_t which will be used to set the current runtime state
     * of *this.
     */
    void set_state(game_state_status_t s);

    /*
     * @brief Get the parent subsystem that manages *this (const).
     *
     * @return A reference to *this object's managing lsSubsystem.
     */
    const GameSystem& get_parent_system() const;

    /*
     * @brief Get the parent subsystem that manages *this.
     *
     * @return A reference to *this object's managing lsSubsystem.
     */
    GameSystem& get_parent_system();

    /**
     * @brief Check if the current state is running.
     *
     * @return True if this object's current state is set to
     * game_state_t::RUNNING, return false if not.
     */
    virtual bool is_running() const;

    /**
     * @brief Check if the current state is paused.
     *
     * @return True if this object's current state is set to
     * game_state_t::PAUSED, return false if not.
     */
    virtual bool is_paused() const;

    /**
     * @brief Check if the current state is initializing.
     *
     * @return True if this object's current state is set to
     * game_state_t::STARTING, return false if not.
     */
    virtual bool is_starting() const;

    /**
     * @brief Check if the current state is about to stop.
     *
     * @return True if this object's current state is set to
     * game_state_t::STOPPING, return false if not.
     */
    virtual bool is_stopping() const;

    /**
     * @brief Check if the current state is stopped.
     *
     * @return True if this object's current state is set to
     * game_state_t::STOPPED, return false if not.
     */
    virtual bool is_stopped() const;

    /**
     * @brief Tell the parent GameSystem that *this should stop.
     */
    virtual void stop_state();

    /**
     * @brief Tell the parent GameSystem that *this should pause.
     */
    virtual void pause_state();

    /**
     * @brief Tell the parent GameSystem that *this should resume from a pause.
     */
    virtual void resume_state();
};



/*-------------------------------------
    Get the current state of *this object.
-------------------------------------*/
inline game_state_status_t GameState::get_state() const
{
    return currentState;
}



/*-------------------------------------
    Set the operational state of *this.
-------------------------------------*/
inline void GameState::set_state(game_state_status_t s)
{
    currentState = s;
}



/*-------------------------------------
    Get the parent subsystem that manages *this (const).
-------------------------------------*/
inline const GameSystem& GameState::get_parent_system() const
{
    return *pSystem;
}



/*-------------------------------------
    Get the parent subsystem that manages *this.
-------------------------------------*/
inline GameSystem& GameState::get_parent_system()
{
    return *pSystem;
}



/*-------------------------------------
    Check if the current state is running.
-------------------------------------*/
inline bool GameState::is_running() const
{
    return currentState == game_state_status_t::RUNNING;
}



/*-------------------------------------
    Check if the current state is paused.
-------------------------------------*/
inline bool GameState::is_paused() const
{
    return currentState == game_state_status_t::PAUSED;
}



/*-------------------------------------
    Check if the current state is initializing.
-------------------------------------*/
inline bool GameState::is_starting() const
{
    return currentState == game_state_status_t::STARTING;
}



/*-------------------------------------
    Check if the current state is stopping.
-------------------------------------*/
inline bool GameState::is_stopping() const
{
    return currentState == game_state_status_t::STOPPING;
}



/*-------------------------------------
    Check if the current state is stopped.
-------------------------------------*/
inline bool GameState::is_stopped() const
{
    return currentState == game_state_status_t::STOPPED;
}



/*-------------------------------------
    Check if the current state is stopped.
-------------------------------------*/
inline void GameState::stop_state()
{
    currentState = game_state_status_t::STOPPING;
}



/*-------------------------------------
    Check if the current state is stopped.
-------------------------------------*/
inline void GameState::pause_state()
{
    currentState = game_state_status_t::PAUSED;
}



/*-------------------------------------
    Check if the current state is stopped.
-------------------------------------*/
inline void GameState::resume_state()
{
    currentState = game_state_status_t::RUNNING;
}



} // end game namespace
} // end ls namespace

#endif  /* LS_GAME_GAME_STATE_H */
