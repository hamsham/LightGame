/*
 * File:   game/dispatcher.cpp
 * Author: miles
 *
 * Created on November 20, 2014, 10:27 PM
 */

#include <utility> // std::move()

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
Dispatcher::~Dispatcher()
{
    for (Subscriber* s : mSubscribers)
    {
        s->mSubscriptions.erase(this);
        s->disconnected(*this);
    }
}



/*-------------------------------------
 * Constructor
-------------------------------------*/
Dispatcher::Dispatcher() :
    mEvents{},
    mSubscribers{}
{}



/*-------------------------------------
 * Move Constructor
-------------------------------------*/
Dispatcher::Dispatcher(Dispatcher&& d) :
    mEvents{std::move(d.mEvents)},
    mSubscribers{std::move(d.mSubscribers)}
{
    for (Subscriber* s : mSubscribers)
    {
        s->mSubscriptions.erase(&d);
        s->disconnected(d);

        s->mSubscriptions.insert(this);
        s->connected(*this);
    }
}



/*-------------------------------------
 * Move Operator
-------------------------------------*/
Dispatcher& Dispatcher::operator=(Dispatcher&& d)
{
    std::unordered_set<Subscriber*> oldSubs = std::move(mSubscribers);
    mSubscribers.clear();

    for (Subscriber* s : oldSubs)
    {
        s->mSubscriptions.erase(this);
        s->disconnected(*this);
    }

    std::unordered_set<Subscriber*> newSubs = std::move(d.mSubscribers);

    for (Subscriber* s : newSubs)
    {
        // always remove the subscriber before notifying it of the removal.
        // This will allow it to attempt a reconnection.
        s->mSubscriptions.erase(&d);
        s->disconnected(d);

        // in case any subscriber removes itself from *this upon creation of
        // a connection, insert here
        mSubscribers.insert(s);
        s->mSubscriptions.insert(this);
        s->connected(*this);
    }

    return *this;
}



/*-------------------------------------
 * dispatch queued events to subscribers
-------------------------------------*/
void Dispatcher::dispatch()
{
    size_t sentinel = mEvents.size();

    for (size_t i = 0; i < sentinel; ++i)
    {
        for (Subscriber* s : mSubscribers)
        {
            s->notified(*this, mEvents[i]);
        }
    }

    size_t newMsgIter = sentinel, oldMsgIter = 0;
    while (newMsgIter != mEvents.size())
    {
        mEvents[oldMsgIter++] = mEvents[newMsgIter++];
    }

    mEvents.erase(mEvents.begin() + oldMsgIter, mEvents.end());
}



} // end game namespace
} // end ls namespace
