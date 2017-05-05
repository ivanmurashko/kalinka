/**
   @file inudp.cpp
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
   - 2009/05/22 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "inudp.h"
#include "exception.h"

using namespace klk;
using namespace klk::http;

//
// InUDPThread class
//

// Constructor
InUDPThread::InUDPThread(Factory* factory, const InputInfoPtr& info) :
    InThread(factory, info), m_sem(), m_init_lock()
{
    BOOST_ASSERT(getInfo()->getRouteInfo()->getProtocol() == sock::UDP);
}

// Destructor
InUDPThread::~InUDPThread()
{
}

// Do initialization before thread startup
void InUDPThread::init()
{
    Locker lock(&m_lock);
    // check
    BOOST_ASSERT(getRoute()->getHost().empty() == false);
    BOOST_ASSERT(getRoute()->getPort() > 0);

    // reset listener
    resetListener();

    // basic initialization
    Thread::init();
}

// stops thread execution
void InUDPThread::stop() throw()
{
    // do ususal actions
    InThread::stop();
    // stop semaphore
    m_sem.post();
}

// Inits reader for UDP connections
void InUDPThread::initReader()
{
    // Before we start wait we should free listener staff
    // unjoin multicast groupo for example
    resetListener();
    // wait until a connection thread appear for the input
    Locker lock(&m_init_lock);
    m_sem.wait();
    if (!isStopped())
    {
        initListener();
        // do typical initialization
        InThread::initReader();
        // restore prev state to have the sem in unlocked state
        m_sem.post();
    }
}

// Main loop (data processing) for UDP connections
void InUDPThread::doLoop()
{
    initReader();

    while (!isStopped())
    {
        // do something only if we have a connectons for this input
        if (m_sem.isLocked())
        {
            // just log
            klk_log(KLKLOG_DEBUG,
                    "No any output connection "
                    "for input HTTP thread on %s:%d. "
                    "Stop receiving info",
                    getRoute()->getHost().c_str(),
                    getRoute()->getPort());
            break;
        }

        doLoopAction();
    }
}

// Increases connection count
void InUDPThread::increaseConnectionCount()
{
    klk_log(KLKLOG_DEBUG, "HTTP: Got connection for UDP input");
    // increase counter
    InThread::increaseConnectionCount();
    // do some IPC sync
    m_sem.post();
    // wait until reader initialization will be finished
    // @see InUDPThread::initReader()
    Locker lock(&m_init_lock);
}

// Decreases connection count
void InUDPThread::decreaseConnectionCount()
{
    klk_log(KLKLOG_DEBUG, "HTTP: Lost connection for UDP input");
    // decrease counter
    InThread::decreaseConnectionCount();
    // do some IPC sync
    m_sem.trywait();
}
