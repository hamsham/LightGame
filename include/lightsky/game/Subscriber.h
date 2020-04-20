/*
 * File:   game/subscriber.h
 * Author: miles
 *
 * Created on November 20, 2014, 10:24 PM
 */

#ifndef LS_GAME_SUBSCRIBER_H
#define LS_GAME_SUBSCRIBER_H

#include <unordered_set>

#include "lightsky/setup/Api.h"

#include "lightsky/game/Event.h"



namespace ls
{
namespace game
{



/**
 * @brief Event Subscription Object (abstract)
 *
 * The Subscription object is a simple event handler which will receive events
 * from one or more dispatchers. When a dispatch object is passed into *this,
 * a connection will remain until either the dispatcher or subscriber has gone
 * out of scope. This permits events to be safely passed around without
 * worrying about scoping issues.
 *
 * Event mSubscribers must be derived and have the virtual "notified()"
 * function overridden.
*/
class Subscriber
{
    friend class Dispatcher;

  private:
    /**
     * @brief pParent is a pointer to the dispatcher to which this
     * subscriber is receiving events from.
     */
    std::unordered_set<Dispatcher*> mSubscriptions;

  protected:

    /**
     * @brief Function to indicate a Dispatcher has been connected.
     *
     * @param d
     * A const reference to the newly-disconnected Dispatcher.
     */
    virtual void connected(Dispatcher& d);

    /**
     * @brief The HandleEvents function allows derived subscriber objects
     * to determine what to do upon receiving an event.
     *
     * @param evt
     * A constant reference to an event object, sent by a call to *this
     * object's parent dispatcher.
     */
    virtual void notified(Dispatcher& d, const Event& evt) = 0;

    /**
     * @brief Function to indicate a connected Dispatcher has been lost and
     * is no longer providing events to *this.
     *
     * @param d
     * A const reference to the now-disconnected Dispatcher.
     */
    virtual void disconnected(const Dispatcher& d);

  public:
    /**
     * @brief Destructor
     *
     * Calls "unsubscribe()" and severs the connection to all dispatchers.
     */
    virtual ~Subscriber() = 0;

    /**
     * @brief Constructor
     *
     * Constructs all members in *this.
     */
    Subscriber();

    /**
     * @brief Copy Constructor
     *
     * This constructor will retrieve the held dispatchers from the input
     * parameter and assign *this as a subscriber. This function will
     * unsubscribe all prior subscriptions.
     *
     * @param s
     * A constant reference to another subscriber object who's internal
     * dispatchers will be shared with *this.
     */
    Subscriber(const Subscriber& s);

    /**
     * @brief Move Constructor
     *
     * This method will remove all dispatchers from *this and acquire the
     * subscriptions from "s". Afterwards, "s" will be unsubscribed from all
     * of its referenced dispatchers.
     *
     * @param s
     * An r-value reference to another subscriber object who's internal
     * dispatchers will be reassigned to *this.
     */
    Subscriber(Subscriber&& s);

    /**
     * @brief Copy Operator
     *
     * This operator will retrieve the held dispatchers from the input
     * parameter and assign *this as a subscriber. This function will
     * unsubscribe all prior subscriptions.
     *
     * @param s
     * A constant reference to another subscriber object who's internal
     * dispatchers will be shared with *this.
     *
     * @return a reference to *this.
     */
    Subscriber& operator=(const Subscriber& s);

    /**
     * @brief Move Operator
     *
     * This method will remove all dispatchers from *this and acquire the
     * subscriptions from "s". Afterwards, "s" will be unsubscribed from all
     * of its referenced dispatchers.
     *
     * @param s
     * An r-value reference to another subscriber object who's internal
     * dispatchers will be reassigned to *this.
     *
     * @return a reference to *this.
     */
    Subscriber& operator=(Subscriber&& s);

    /**
     * @brief Set the parent dispatcher for *this.
     *
     * This function will create a connection to a dispatcher object in
     * order to receive events from it.
     *
     * @param d
     * A dispatch object who will now publish events to *this.
     */
    void subscribe(Dispatcher& d);

    /**
     * @brief Unsubscribe from a dispatcher's notifications.
     *
     * This function will sever tje connection to a specific dispatcher object
     * if it is already referenced by *this. This function does nothing if
     * the input dispatcher was not already subscribed.
     *
     * @param d
     * A const reference to a dispatcher object.
     */
    void unsubscribe(Dispatcher& d);

    /**
     * @brief Unsubscribe from all dispatched communications.
     *
     * This function will sever the connections to all dispatcher objects
     * referenced by *this.
     */
    void unsubscribe();

    /**
     * @brief Determine if *this is subscribed to a dispatcher.
     *
     * @param d
     * A constant reference to a dispatcher object.
     *
     * @return TRUE if *this is subscribed to the input dispatcher, FALSE
     * if otherwise.
     */
    bool is_subscribed(const Dispatcher& d) const;
};



} // end game namespace
} // end ls namespace

#endif  /* LS_GAME_SUBSCRIBER_H */
