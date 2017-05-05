/**
   @file thread.cpp
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
   - 2008/06/08 created by ipp (Ivan Murashko)
   - 2009/01/01 klk namespace introduced by Ivan Murashko
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <string.h>

#include "thread.h"
#include "log.h"
#include "utils.h"
#include "exception.h"

using namespace klk;
using namespace klk::base;

//
// Mutex
//

// Constructor
Mutex::Mutex()
{
    // Init our recursive mutex (Note: GNU code)
    pthread_mutexattr_init(&m_attr);
    pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_mutex, &m_attr);
}

// Destructor
Mutex::~Mutex()
{
    //free mutex
    pthread_mutex_destroy(&m_mutex);
    // free attributes
    pthread_mutexattr_destroy(&m_attr);
}

//
// Locker class
//

// Constructor
// @param[in] thread thread to be locked
Locker::Locker(Mutex* thread) throw() : m_mutex(NULL)
{
    try
    {
        BOOST_ASSERT(thread);
        m_mutex = thread->getMutex();
        BOOST_ASSERT(m_mutex);
        pthread_mutex_lock(m_mutex);
    }
    catch(...)
    {
        KLKASSERT(0);
    }
}

// Destructor
// Unlocks the thread
Locker::~Locker()
{
    try
    {
        BOOST_ASSERT(m_mutex);
        pthread_mutex_unlock(m_mutex);
    }
    catch(...)
    {
        KLKASSERT(0);
    }
}

//
// Event
//

// Constructor
Event::Event() :
    m_exit(false), m_loop(false)
{
    if (pthread_condattr_init(&m_wait_attr))
        return;

    if (pthread_mutex_init(&m_wait_mutex, NULL))
    {
        pthread_condattr_destroy(&m_wait_attr);
        return;
    }

    if (pthread_cond_init(&m_wait_cond, &m_wait_attr))
    {
        pthread_condattr_destroy(&m_wait_attr);
        return;
    }

    pthread_condattr_destroy(&m_wait_attr);
}

// Destructor
Event::~Event()
{
    stopWait();
    pthread_mutex_destroy(&m_wait_mutex);
    pthread_cond_destroy(&m_wait_cond);
}

// Does a lock
void Event::lock() throw()
{
    pthread_mutex_lock(&m_wait_mutex);
}

// Does an unlock
void Event::unlock() throw()
{
    pthread_mutex_unlock(&m_wait_mutex);
}


// Starts wait process
// @param[in] interval - the wait interval
Result Event::startWait(u_long interval)
{
    Result rc = OK;
    // wait for awhile
    struct timespec to;
    to.tv_sec = time(NULL) + interval;
    to.tv_nsec = 0;

    lock();
    BOOST_ASSERT(m_loop == false);
    while (!m_exit)
    {
        m_loop = true;
        if (interval != 0)
        {
            int ret = pthread_cond_timedwait(&m_wait_cond,
                                             &m_wait_mutex, &to);
            if (/* ( ret == ETIME ) || */( ret == ETIMEDOUT ))
            {
                rc = ERROR; // timeout exceed
                break;
            }

            if (ret != EINTR)
            {
                break;
            }
        }
        else
        {
            pthread_cond_wait(&m_wait_cond, &m_wait_mutex);
        }
    }
    m_loop = false;
    m_exit = false;
    unlock();

    return rc;
}

// Stops waiting
void Event::stopWait() throw()
{
    lock();
    if (m_loop)
    {
        m_exit = true;
        if (pthread_cond_broadcast(&m_wait_cond) != 0)
        {
            klk_log(KLKLOG_ERROR, "Error %d in pthread_cond_broadcast(): %s",
                    errno, strerror(errno));
        }
    }
    unlock();
}

//
// Trigger class
//

// Constructor
Trigger::Trigger() : m_sem()
{
}

// Destructor
Trigger::~Trigger()
{
}

// Is it stopped or not
bool Trigger::isStopped()
{
    // stop means not locked
    return !m_sem.isLocked();
}

// wait for a stop event
void Trigger::wait()
{
    m_sem.wait();
    // restore stopped state
    m_sem.post();
}


// Do the stop for the trigger
void Trigger::stop() throw()
{
    try
    {
        m_sem.post();
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Trigger::stop(): error");
    }
}

// Inits the trigger
void Trigger::init()
{
    m_sem.init();
}

//
// Thread class
//

// Constructor
Thread::Thread() :
    m_lock(), m_stop()
{
}

// Destructor
Thread::~Thread()
{
}

// Initializes the thread info
void Thread::init()
{
    m_stop.init();
}

// Stops the thread
void Thread::stop() throw()
{
    m_stop.stop();
}

// Check if the thread stopped
bool Thread::isStopped()
{
    return m_stop.isStopped();
}
