/*
 * File:   game/subscriber.cpp
 * Author: miles
 *
 * Created on November 20, 2014, 10:24 PM
 */

#include "lightsky/game/Event.h"
#include "lightsky/game/Subscriber.h"
#include "lightsky/game/Dispatcher.h"

namespace ls {
namespace game {

/*-------------------------------------
 * Destructor
-------------------------------------*/
Subscriber::~Subscriber() {
    setDispatcher(nullptr);
}

/*-------------------------------------
 * Constructor
-------------------------------------*/
Subscriber::Subscriber() {
}

/*-------------------------------------
 * Copy Constructor
-------------------------------------*/
Subscriber::Subscriber(const Subscriber& s) {
    setDispatcher(s.pParent);
}

/*-------------------------------------
 * Move Constructor
-------------------------------------*/
Subscriber::Subscriber(Subscriber&& s) {
    setDispatcher(s.pParent);
    s.setDispatcher(nullptr);
}

/*-------------------------------------
 * Copy Operator
-------------------------------------*/
Subscriber& Subscriber::operator=(const Subscriber& s) {
    setDispatcher(s.pParent);
    return *this;
}

/*-------------------------------------
 * Move Operator
-------------------------------------*/
Subscriber& Subscriber::operator=(Subscriber&& s) {
    setDispatcher(s.pParent);
    s.setDispatcher(nullptr);
    return *this;
}

/*-------------------------------------
 * Set the parent dispatcher
-------------------------------------*/
void Subscriber::setDispatcher(Dispatcher* const pDispatcher) {
    if (pParent == pDispatcher) {
        return;
    }

    if (pParent != nullptr) {
        pParent->unassignSubscriber(*this);
    }

    pParent = pDispatcher;
    if (pParent != nullptr) {
        pParent->assignSubscriber(*this);
    }
}

/*-------------------------------------
 * Confirm a parent dispatcher
-------------------------------------*/
bool Subscriber::isSubscribed(const Dispatcher& d) const {
    return pParent == &d;
}

} // end game namespace
} // end ls namespace
