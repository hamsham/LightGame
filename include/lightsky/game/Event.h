/*
 * File:   event.h
 * Author: miles
 *
 * Created on November 20, 2014, 10:22 PM
 */

#ifndef LS_GAME_EVENT_H
#define LS_GAME_EVENT_H

#include <cstdint>



namespace ls
{
namespace game
{



/**----------------------------------------------------------------------------
 * @brief Basic event structure.
-----------------------------------------------------------------------------*/
struct alignas(sizeof(uint64_t)) Event
{
    /**
     * @brief A event's baseId should contain enough information about a
     * event to identify where an event came from.
     *
     * For example, a base ID could be something similar to "KEYBOARD_EVENT"
     * while the action ID is "KEY_BUTTON_PRESSED."
     */
    uint64_t type;

    /**
     * @brief The action id should help mSubscribers determine what action was
     * performed by the event source.
     */
    uint64_t info;

    /**
     * @brief Extra event data 1.
     */
    uint64_t extra1;

    /**
     * @brief Extra event data 2.
     */
    uint64_t extra2;
};



} // end game namespace
} // end ls namespace

#endif  /* LS_GAME_EVENT_H */
