/*
 * File:   event.h
 * Author: miles
 *
 * Created on November 20, 2014, 10:22 PM
 */

#ifndef __LS_GAME_EVENT_H__
#define __LS_GAME_EVENT_H__

#include <cstdint>

namespace ls {
namespace game {

/**----------------------------------------------------------------------------
 * Forward declarations
-----------------------------------------------------------------------------*/
class Subscriber;
class Dispatcher;



/**----------------------------------------------------------------------------
 * Basic event structure.
-----------------------------------------------------------------------------*/
struct Event {
    /**
     * @brief pSource contains a pointer to the source of a event.
     */
    Dispatcher* pSrc;

    /**
     * @brief A event's baseId should contain enough information about a
     * event to identify where an event came from.
     *
     * For example, a base ID could be something similar to "KEYBOARD_EVENT"
     * while the action ID is "KEY_BUTTON_PRESSED."
     */
    uint64_t type;

    /**
     * @brief The action id should help subscribers determine what action was
     * performed by the event source.
     */
    uint64_t info;

    /**
     * @brief pData contains a pointer to any data from within the source
     * dispatcher.
     */
    void* pExtra;
};

} // end game namespace
} // end ls namespace

#endif  /* __LS_GAME_EVENT_H__ */
