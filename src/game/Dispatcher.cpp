/*
 * File:   game/dispatcher.cpp
 * Author: miles
 *
 * Created on November 20, 2014, 10:27 PM
 */

#include "lightsky/game/Event.h"
#include "lightsky/game/Subscriber.h"
#include "lightsky/game/Dispatcher.h"

namespace ls {
namespace game {

/*-------------------------------------
 * Destructor
-------------------------------------*/
Dispatcher::~Dispatcher() {
    clearSubscribers();
}

/*-------------------------------------
 * Constructor
-------------------------------------*/
Dispatcher::Dispatcher() :
    events{},
    subscribers{}
{}

/*-------------------------------------
 * Move Constructor
-------------------------------------*/
Dispatcher::Dispatcher(Dispatcher&& d) :
    events{std::move(d.events)},
    subscribers{std::move(d.subscribers)}
{
    typename subscriberMap_t::iterator iter = subscribers.begin();
    while (iter != subscribers.end()) {
        Subscriber* const pSubscriber = iter->second;
        pSubscriber->pParent = this;
        ++iter;
    }
}

/*-------------------------------------
 * Move Operator
-------------------------------------*/
Dispatcher& Dispatcher::operator=(Dispatcher&& d) {
    events = std::move(d.events);
    subscribers = std::move(d.subscribers);

    typename subscriberMap_t::iterator iter = subscribers.begin();
    while (iter != subscribers.end()) {
        Subscriber* const pSubscriber = iter->second;
        pSubscriber->pParent = this;
        ++iter;
    }
    return *this;
}

/*-------------------------------------
 * Assign a subscriber
-------------------------------------*/
void Dispatcher::assignSubscriber(Subscriber& s) {
    Subscriber* const pSubscriber = &s;
    pSubscriber->pParent = this;
    subscribers[pSubscriber] = pSubscriber;
}

/*-------------------------------------
 * Remove a subscriber
-------------------------------------*/
void Dispatcher::unassignSubscriber(Subscriber& s) {
    if (subscribers.erase(&s) != 0) { // insurance
        s.pParent = nullptr;
    }
}

/*-------------------------------------
 * Check if dispatching to a subscriber
-------------------------------------*/
bool Dispatcher::hasSubscriber(const Subscriber& s) const {
    return s.pParent == this;
}

/*-------------------------------------
 * remove all subscribers from the distribution list
-------------------------------------*/
void Dispatcher::clearSubscribers() {
    typename subscriberMap_t::iterator iter = subscribers.begin();

    while (iter != subscribers.end()) {
        Subscriber* const pSubscriber = iter->second;
        pSubscriber->pParent = nullptr;
        ++iter;
    }
    subscribers.clear();
}

/*-------------------------------------
 * dispatch events to subscribers
-------------------------------------*/
void Dispatcher::dispatchEvents() {
    const unsigned sentinel = events.size();

    for (unsigned i = 0; i < sentinel; ++i) {
        typename subscriberMap_t::iterator iter = subscribers.begin();
        while (iter != subscribers.end()) {
            Subscriber* const pSubscriber = iter->second;
            pSubscriber->handleEvent(events[i]);
            ++iter;
        }
    }
    events.erase(events.begin(), events.begin()+sentinel);
}

/*-------------------------------------
 * push an event to the event queue
-------------------------------------*/
void Dispatcher::pushEvent(const Event& t) {
    events.push_back(t);
}


/*-------------------------------------
 * get the number of queued events
-------------------------------------*/
unsigned Dispatcher::getNumEventsQueued() const {
    return events.size();
}

/*-------------------------------------
 * get the number of subscribers
-------------------------------------*/
unsigned Dispatcher::getNumSubscribers() const {
    return subscribers.size();
}

} // end game namespace
} // end ls namespace
