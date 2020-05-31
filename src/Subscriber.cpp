/*
 * File:   game/subscriber.cpp
 * Author: miles
 *
 * Created on November 20, 2014, 10:24 PM
 */

#include <utility> // std::move()

#include "lightsky/utils/Assertions.h"

#include "lightsky/game/Event.h"
#include "lightsky/game/Subscriber.h"
#include "lightsky/game/Dispatcher.h"

namespace ls
{
namespace game
{



/*-------------------------------------
 * Destructor
-------------------------------------*/
Subscriber::~Subscriber()
{
    unsubscribe();
}



/*-------------------------------------
 * Constructor
-------------------------------------*/
Subscriber::Subscriber() :
    mSubscriptions{}
{}



/*-------------------------------------
 * Copy Constructor
-------------------------------------*/
Subscriber::Subscriber(const Subscriber& s) :
    mSubscriptions{}
{
    for (Dispatcher* pDispatcher : s.mSubscriptions)
    {
        subscribe(*pDispatcher);
    }
}



/*-------------------------------------
 * Move Constructor
-------------------------------------*/
Subscriber::Subscriber(Subscriber&& s)
{
    for (Dispatcher* pDispatcher : s.mSubscriptions)
    {
        subscribe(*pDispatcher);
    }

    s.unsubscribe();
}



/*-------------------------------------
 * Copy Operator
-------------------------------------*/
Subscriber& Subscriber::operator=(const Subscriber& s)
{
    unsubscribe();

    for (Dispatcher* pDispatcher : s.mSubscriptions)
    {
        subscribe(*pDispatcher);
    }

    return *this;
}



/*-------------------------------------
 * Move Operator
-------------------------------------*/
Subscriber& Subscriber::operator=(Subscriber&& s)
{
    unsubscribe();

    for (Dispatcher* pDispatcher : s.mSubscriptions)
    {
        subscribe(*pDispatcher);
    }

    s.unsubscribe();

    return *this;
}



/*-------------------------------------
 * Connection Event
-------------------------------------*/
void Subscriber::connected(Dispatcher& d)
{
    (void)d;
}



/*-------------------------------------
 * Disconnection Event
-------------------------------------*/
void Subscriber::disconnected(const Dispatcher& d)
{
    (void)d;
}



/*-------------------------------------
 * Add a new subscription
-------------------------------------*/
void Subscriber::subscribe(Dispatcher& d)
{
    if (is_subscribed(d))
    {
        return;
    }

    LS_DEBUG_ASSERT(d.mSubscribers.count(this) == 0);
    d.mSubscribers.insert(this);
    mSubscriptions.insert(&d);
}



/*-------------------------------------
 * Remove a subscription
-------------------------------------*/
void Subscriber::unsubscribe(Dispatcher& d)
{
    if (!is_subscribed(d))
    {
        return;
    }

    LS_DEBUG_ASSERT(d.mSubscribers.count(this));

    d.mSubscribers.erase(this);
    mSubscriptions.erase(&d);
}



/*-------------------------------------
 * Remove all subscription
-------------------------------------*/
void Subscriber::unsubscribe()
{
    for (Dispatcher* d : mSubscriptions)
    {
        LS_DEBUG_ASSERT(d->mSubscribers.count(this));
        d->mSubscribers.erase(this);
    }

    while (!mSubscriptions.empty())
    {
        mSubscriptions.erase(mSubscriptions.begin());
    }
}



/*-------------------------------------
 * Confirm a parent dispatcher
-------------------------------------*/
bool Subscriber::is_subscribed(const Dispatcher& d) const
{
    return mSubscriptions.count(const_cast<Dispatcher*>(&d)) != 0;
}



} // end game namespace
} // end ls namespace
