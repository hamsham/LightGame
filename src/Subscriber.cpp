/*
 * File:   game/subscriber.cpp
 * Author: miles
 *
 * Created on November 20, 2014, 10:24 PM
 */

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
    set_dispatcher(nullptr);
}



/*-------------------------------------
 * Constructor
-------------------------------------*/
Subscriber::Subscriber()
{
}



/*-------------------------------------
 * Copy Constructor
-------------------------------------*/
Subscriber::Subscriber(const Subscriber& s)
{
    set_dispatcher(s.pParent);
}



/*-------------------------------------
 * Move Constructor
-------------------------------------*/
Subscriber::Subscriber(Subscriber&& s)
{
    set_dispatcher(s.pParent);
    s.set_dispatcher(nullptr);
}



/*-------------------------------------
 * Copy Operator
-------------------------------------*/
Subscriber& Subscriber::operator=(const Subscriber& s)
{
    set_dispatcher(s.pParent);
    return *this;
}



/*-------------------------------------
 * Move Operator
-------------------------------------*/
Subscriber& Subscriber::operator=(Subscriber&& s)
{
    set_dispatcher(s.pParent);
    s.set_dispatcher(nullptr);
    return *this;
}



/*-------------------------------------
 * Set the parent dispatcher
-------------------------------------*/
void Subscriber::set_dispatcher(Dispatcher* const pDispatcher)
{
    if (pParent == pDispatcher)
    {
        return;
    }

    if (pParent != nullptr)
    {
        pParent->remove_subscriber(*this);
    }

    pParent = pDispatcher;
    if (pParent != nullptr)
    {
        pParent->add_subscriber(*this);
    }
}



/*-------------------------------------
 * Confirm a parent dispatcher
-------------------------------------*/
bool Subscriber::is_subscribed(const Dispatcher& d) const
{
    return pParent == &d;
}



} // end game namespace
} // end ls namespace
