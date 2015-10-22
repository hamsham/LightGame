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
    clear_subscribers();
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
void Dispatcher::add_subscriber(Subscriber& s) {
    Subscriber* const pSubscriber = &s;
    pSubscriber->pParent = this;
    subscribers[pSubscriber] = pSubscriber;
}

/*-------------------------------------
 * Remove a subscriber
-------------------------------------*/
void Dispatcher::remove_subscriber(Subscriber& s) {
    if (subscribers.erase(&s) != 0) { // insurance
        s.pParent = nullptr;
    }
}

/*-------------------------------------
 * Check if dispatching to a subscriber
-------------------------------------*/
bool Dispatcher::has_subscriber(const Subscriber& s) const {
    return s.pParent == this;
}

/*-------------------------------------
 * remove all subscribers from the distribution list
-------------------------------------*/
void Dispatcher::clear_subscribers() {
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
void Dispatcher::dispatch_events() {
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
void Dispatcher::push_event(const Event& t) {
    events.push_back(t);
}


/*-------------------------------------
 * get the number of queued events
-------------------------------------*/
unsigned Dispatcher::get_num_queued_events() const {
    return events.size();
}

/*-------------------------------------
 * get the number of subscribers
-------------------------------------*/
unsigned Dispatcher::get_num_subscribers() const {
    return subscribers.size();
}

} // end game namespace
} // end ls namespace
