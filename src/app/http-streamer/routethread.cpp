/**
   @file routethread.cpp
   @brief This file is part of Kalinka mediaserver.
   @author Ivan Murashko <ivan.murashko@gmail.com>

   Copyright (c) 2007-2012 Kalinka Team

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   CHANGE HISTORY

   @date
   - 2009/03/09 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "routethread.h"
#include "exception.h"
#include "httpfactory.h"

using namespace klk;
using namespace klk::http;

//
// Thread class
//

// Constructor
Thread::Thread(Factory* factory) :
    base::Thread(), Base(factory)
{
}

// Destructor
Thread::~Thread()
{
}

//
// RouteThread class
//

// Constructor
RouteThread::RouteThread(Factory* factory) :
    Thread(factory),
    m_listener(),
    m_route()
{
}

// Destructor
RouteThread::~RouteThread()
{
}

// Setups route
void RouteThread::setRoute(const RouteInfoPtr& route)
{
    BOOST_ASSERT(route);
    // the route should not be set before
    if (checkRoute(route->getUUID()))
    {
        throw Exception(__FILE__, __LINE__,
                        "Route has been already set: " + route->getUUID());
    }
    // some data check
    BOOST_ASSERT(route->getHost().empty() == false);
    BOOST_ASSERT(route->getPort() > 0);
    Locker lock(&m_lock);
    m_route = route;
}

// Retrive route host
const RouteInfoPtr RouteThread::getRoute() const
{
    Locker lock(&m_lock);
    if (!m_route)
    {
        throw Exception(__FILE__, __LINE__, "Route is not initialized");
    }
    return m_route;
}

// Inits the thread
void RouteThread::init()
{
    // some checking
    {
        Locker lock(&m_lock);
        BOOST_ASSERT(m_route);
        BOOST_ASSERT(m_route->getHost().empty() == false);
        BOOST_ASSERT(m_route->getPort() > 0);
    }

    initListener();

    Thread::init();
}

// Retrives listener pointer
const IListenerPtr RouteThread::getListener() const
{
    Locker lock(&m_lock);
    BOOST_ASSERT(m_listener);
    return m_listener;
}


// Inits listener
void RouteThread::initListener()
{
    Locker lock(&m_lock);
    BOOST_ASSERT(m_route);
    m_listener = sock::Factory::getListener(*m_route);
}

// resets listener
void RouteThread::resetListener() throw()
{
    Locker lock(&m_lock);
    m_listener.reset();
}

// Stops the thread
void RouteThread::stop() throw()
{
    Thread::stop();
    try
    {
        getListener()->stop();
    }
    catch(...)
    {
        // nothing to do
        // no listener initialized
    }
}

// Checks is the route (at the args) has been already set
const bool RouteThread::checkRoute(const std::string& uuid) const throw()
{
    Locker lock(&m_lock);
    if (!m_route)
    {
        return false;
    }
    return (m_route->getUUID() == uuid);
}

// Checks is there any route has been already set
const bool RouteThread::checkRoute() const throw()
{
    Locker lock(&m_lock);
    return m_route;
}
