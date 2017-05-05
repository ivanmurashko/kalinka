/**
   @file scheduler.cpp
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
   - 2008/10/25 created by ipp (Ivan Murashko)
   - 2009/01/01 klk namespace introduced by Ivan Murashko
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <signal.h>
#include <errno.h>
#include <string.h>
#include <memory>

#include <mysql.h>

#include <boost/bind.hpp>

#include "modfactory.h"
#include "log.h"
#include "common.h"
#include "exception.h"

using namespace klk;
using namespace klk::base;

//
// Several usefull procs
//
/**
   @brief Stack size default

   A comment from aspseek code:
   seems that index coredumps if thread stack size is short
*/
const u_int NEEDED_THREADSTACK_SIZE = 131072;

// wrapper for pthread_create
static Result klk_pthread_create(pthread_t * thread,
                                 void * (*start_routine)(void *),
                                 void * arg)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, NEEDED_THREADSTACK_SIZE);

    if (pthread_create(thread, &attr,
                       start_routine, arg) != 0)
    {
        // Result
        klk_log(KLKLOG_ERROR,
                "Error %d in pthread_create(): %s",
                errno, strerror(errno));
        KLKASSERT(0);
        return ERROR;
    }

    return OK;
}

// starts a thread
static void* start_thread(void* param)
{
    KLKASSERT(param != NULL);

    IThread* thread = static_cast<IThread*>(param);
    if (thread == NULL)
    {
        KLKASSERT(thread != NULL);
        return NULL;
    }

    // ignore several signals in threads
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGTERM);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGPIPE);
    pthread_sigmask(SIG_SETMASK, &sigset, NULL);

    try
    {
        // all threads should be able to communicate with db
        // check thread safety
        BOOST_ASSERT(mysql_thread_safe() == 1);

        if (mysql_thread_init() != 0)
        {
            throw Exception(__FILE__, __LINE__,
                            "Error in mysql_thread_init()");
        }

        thread->start();
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR, "Got an exception during thread execution: %s",
                err.what());
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR,
                "Got an unknown exception during thread execution");
    }
    mysql_thread_end();

    return NULL;
}

//
// ThreadInfo class
//

// Constructor
ThreadInfo::ThreadInfo(const IThreadPtr& thread, pthread_t id) :
    m_thread(thread), m_id(id)
{
}

// Copy constructor
// @param[in] value - the copy param
ThreadInfo::ThreadInfo(const ThreadInfo& value) :
    m_thread(value.m_thread), m_id(value.m_id)
{
}


// Assigment operator
// @param[in] value - the copy param
ThreadInfo& ThreadInfo::operator=(const ThreadInfo& value)
{
    if (this == &value)
        return *this;
    m_thread = value.m_thread;
    m_id = value.m_id;
    return *this;
}

// Compare operator
// @param[in] value - the value to be compared
bool ThreadInfo::operator==(const ThreadInfo& value) const
{
    if (*this == value.m_thread)
    {
        KLKASSERT(m_id == value.m_id);
        return true;
    }

    return false;
}

// Compare operator
// @param[in] value - the value to be compared
bool ThreadInfo::operator==(const IThreadPtr& thread) const
{
    if (m_thread == thread)
        return true;
    KLKASSERT(m_thread);
    return false;
}


// Do the join the thread
void ThreadInfo::join() const
{
    void* res = NULL;
    int rc = 0;
    if ((rc = pthread_join(m_id, &res)) != 0)
    {
        // Error at join just log anf ignore it
        klk_log(KLKLOG_ERROR,
                "Error %d in pthread_join(): %s",
                rc, strerror(rc));
        //KLKASSERT(0);
    }
}

//
// Scheduler class
//

// Constructor
Scheduler::Scheduler()  :
    m_lock(), m_threads(), m_stop()
{
}

// Destructor
Scheduler::~Scheduler()
{
    KLKASSERT(m_threads.empty());
}

// Starts a thread
// @param[in] thread thread to be started
void Scheduler::startThread(const IThreadPtr& thread)
{
    // should be
    BOOST_ASSERT(thread);

    Locker lock(&m_lock);

    if (m_stop.isStopped())
    {
        klk_log(KLKLOG_ERROR,
                "Trying to start a thread during stop all. "
                "It was ignored");
        return;
    }

    // and only one time
    if (isStartedUnsafe(thread))
    {
        return;
    }

    BOOST_ASSERT(isStartedUnsafe(thread) == false);

    // do init first
    thread->init();

    pthread_t pthread_id = 0;
    Result rc = klk_pthread_create(&pthread_id, start_thread,
                                   static_cast<void*>(thread.get()));
    BOOST_ASSERT(rc == OK);

    ThreadInfo info(thread, pthread_id);
    m_threads.push_front(info);

    BOOST_ASSERT(isStartedUnsafe(thread) == true);
}

// Stops a thread
// @param[in] thread to be stopped
void Scheduler::stopThread(const IThreadPtr& thread)
{
    if (m_stop.isStopped())
    {
        // nothing to do
        return;
    }

    // should be
    BOOST_ASSERT(thread);

    // get thread info (locked call)
    std::auto_ptr<ThreadInfo> info(getThreadInfo(thread));
    // stop the thread (unlocked call)
    BOOST_ASSERT(info.get());
    stopThreadByInfo(*info);
    // erase the info (locked call)
    eraseThread(thread);
}

// Checks if the module was already loaded
bool Scheduler::isStarted(const IThreadPtr& thread)
{
    BOOST_ASSERT(thread);

    Locker lock(&m_lock);
    return isStartedUnsafe(thread);
}

// Stops all threads
void Scheduler::stop() throw()
{
    {
        Locker lock(&m_lock);
        m_stop.stop();
    }
    //FIXME!!! bad code
    ThreadInfoList list = getActiveThreads();
    // not locked stop each thread
    std::for_each(list.begin(), list.end(),
                  boost::bind(&Scheduler::stopThreadByInfo, this, _1));
    // clear the list
    Locker lock(&m_lock);
    if (list != m_threads)
    {
        // nothing new was added
        KLKASSERT(list.size() >= m_threads.size());
        for (ThreadInfoList::iterator i = m_threads.begin();
             i != m_threads.end(); i++)
        {
            // should be in list
            KLKASSERT(std::find(list.begin(), list.end(), *i) != list.end());
        }
    }
    m_threads.clear();
}

// Checks if the module was already loaded
bool Scheduler::isStartedUnsafe(const IThreadPtr& thread)
{
    ThreadInfoList::iterator i =
        std::find(m_threads.begin(), m_threads.end(), thread);
    if (i != m_threads.end())
    {
        return true;
    }

    return false;
}

// Retrives the thread list
const ThreadInfoList Scheduler::getActiveThreads() throw()
{
    Locker lock(&m_lock);
    return m_threads;
}

// Stops a thread at the scheduler by means of @ref ThreadInfo object
void Scheduler::stopThreadByInfo(const ThreadInfo& info)
{
    if (info.getThread() == NULL)
    {
        return;
    }

    if (info.getThread()->isStopped())
        return; // has been stopped

    info.getThread()->stop();

    // join
    info.join();
}

// Retrives thread info by thread
ThreadInfo* Scheduler::getThreadInfo(const IThreadPtr& thread)
{
    Locker lock(&m_lock);

    ThreadInfoList::iterator i =
        std::find(m_threads.begin(), m_threads.end(), thread);
    if (i == m_threads.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Could not get a thread by info");
    }
    return new ThreadInfo(*i);
}

// Erases thread info from the threads list
void Scheduler::eraseThread(const IThreadPtr& thread) throw()
{
    Locker lock(&m_lock);

    // remove it from the list
    ThreadInfoList::iterator i =
        std::find(m_threads.begin(), m_threads.end(), thread);
    if (i != m_threads.end())
        m_threads.erase(i);
}

// Retrives thread info list
const ThreadInfoList Scheduler::getThreadInfoList() const
{
    Locker lock(&m_lock);
    return m_threads;
}
