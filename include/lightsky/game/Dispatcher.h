/*
 * File:   game/dispatcher.h
 * Author: miles
 *
 * Created on November 20, 2014, 10:27 PM
 */

#ifndef LS_GAME_DISPATCHER_H
#define LS_GAME_DISPATCHER_H

#include <vector>
#include <unordered_set>

#include "lightsky/setup/Api.h"

#include "lightsky/game/Event.h"



namespace ls
{
namespace game
{



/**
 * @brief Event dispatcher object
 *
 * The dispatcher class contains a queue of events and a table of subscription
 * objects to pass the events to. Events can be pushed to a dispatcher's event
 * queue, then dispatched to all subscribers. Subscribers are automatically
 * dereferenced if this object goes out of their scope (i.e. if a call to
 * "delete" is made or an instance is destroyed).
*/
class Dispatcher
{
    friend class Subscriber;

  private:

    /**
     * @brief events
     *
     * An array of event objects that will be passed to *this object's
     * list of subscribers.
     */
    std::vector<Event> mEvents;

    /**
     * @brief subscribers
     *
     * A map of identifications to subscriber objects. In most cases, the
     * ID at which a subscriber is identified will be a pointer to the
     * subscriber itself. Subscribers are mapped in a table in order to
     * make element access much easier during insertion and deletion.
     */
    std::unordered_set<Subscriber*> mSubscribers;

  public:
    /**
     * @brief Destructor
     *
     * Notifies all subscribers that *this is going out of scope. Clears all
     * internal resources.
     */
    virtual ~Dispatcher();

    /**
     * @brief Constructor
     *
     * Constructs *this with an empty event queue and subscriber list.
     */
    Dispatcher();

    /**
     * @brief Copy Constructor - DELETED
     *
     * The copy constructor has been deleted due to the fact that
     * subscribers need to explicitly add a dispatcher.
     */
    Dispatcher(const Dispatcher& d) = delete;

    /**
     * @brief Move Constructor
     *
     * The move constructor moves all data from the input dispatch object
     * into *this. All child subscribers are then reassigned to *this.
     *
     * @param d
     * An-value reference to another dispatch object who's data will be
     * moved into *this.
     */
    Dispatcher(Dispatcher&& d);

    /**
     * @brief Copy Operator -- DELETED
     *
     * The copy operator has been deleted due to the fact that
     * subscribers need to explicitly add a dispatcher.
     */
    Dispatcher& operator=(const Dispatcher& d) = delete;

    /**
     * @brief Move Operator
     *
     * The move constructor moves all data from the input dispatch object
     * into *this. All child subscribers are then reassigned to *this.
     *
     * @param d
     * An-value reference to another dispatch object who's data will be
     * moved into *this.
     *
     * @return a reference to *this.
     */
    Dispatcher& operator=(Dispatcher&& d);

    /**
     * @brief Dispatch all events contained within the internal event queue.
     *
     * All events contained within *this object's event queue are sent to
     * each subscriber referenced by *this. Once the events are dispatched,
     * the event queue is cleared. to avoid possible circular event passing.
     *
     * This method should be called frequently in order to avoid memory
     * leaks.
     */
    void dispatch();

    /**
     * @brief Bypass the internal event queue and immediately publish an event
     * to all subscribers.
     *
     * @param e
     * A single event to immediately dispatch.
     */
    void dispatch(const Event& e);

    /**
     * @brief Push an event into *this object's event queue.
     *
     * @param e
     * A constant reference to an event object which will be added to the
     * event queue in *this.
     */
    void push(const Event& e);

    /**
     * @brief Retrieve the number of events queued within the event buffer.
     *
     * @return an unsigned integral type, representing the number of events
     * that *this object can distribute to its subscribers.
     */
    size_t num_queued_events() const;

    /**
     * Get the number of subscribers receiving events from *this.
     *
     * @return an unsigned integral type, representing the number of
     * subscriber objects that are receiving events from *this.
     */
    size_t num_subscribers() const;
};



/*-------------------------------------
 * dispatch an immediate event
-------------------------------------*/
inline void Dispatcher::dispatch(const Event& e)
{
    for (Subscriber* s : mSubscribers)
    {
        s->notified(*this, e);
    }
}



/*-------------------------------------
 * Enqueue an event
-------------------------------------*/
inline void Dispatcher::push(const Event& e)
{
    mEvents.push_back(e);
}



/*-------------------------------------
 * Number of enqueued events
-------------------------------------*/
inline size_t Dispatcher::num_queued_events() const
{
    return mEvents.size();
}



/*-------------------------------------
 * Number of subscribers
-------------------------------------*/
inline size_t Dispatcher::num_subscribers() const
{
    return mSubscribers.size();
}



} // end game namespace
} // end ls namespace

#endif  /* LS_GAME_DISPATCHER_H */
