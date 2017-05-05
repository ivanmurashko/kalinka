/**
   @file modulescheduler.cpp
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
   - 2009/01/09 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <mysql.h>

#include <boost/bind.hpp>

#include "modulescheduler.h"
#include "log.h"
#include "exception.h"

using namespace klk;
using namespace klk::mod;

//
// TimerThread class
//

// Constructor
TimerThread::TimerThread(TimerFunction f, time_t intrv) :
    Thread(), m_f(f), m_intrv(intrv), m_wait()
{
}

// starts the thread
void TimerThread::start()
{
    // wait first
    m_wait.startWait(m_intrv);

    while (!isStopped())
    {
        try
        {
            // do the action
            m_f();
        }
        catch(const std::exception& err)
        {
            klk_log(KLKLOG_ERROR,
                    "Got an error in timer thread: %s",
                    err.what());
        }
        catch(...)
        {
            klk_log(KLKLOG_ERROR, "Got an unspecified error in timer thread");
        }

        m_wait.startWait(m_intrv);
    }
}

// stops the thread
void TimerThread::stop() throw()
{
    Thread::stop();
    m_wait.stopWait();
}

//
// Scheduler class
//

// Constructor
Scheduler::Scheduler() :
    base::Scheduler(), m_list()
{
}

// Adds a thread to be started
void Scheduler::addThread(const IThreadPtr& thread)
{
    Locker lock(&m_lock);
    BOOST_ASSERT(thread);
    BOOST_ASSERT(std::find(m_list.begin(), m_list.end(), thread) ==
                 m_list.end());
    m_list.push_back(thread);
}

// @brief Starts the scheduler
void Scheduler::start()
{
    clear(); // to be able restart a module

    std::for_each(m_list.begin(), m_list.end(),
                  boost::bind(&Scheduler::startThread, this, _1));
}
