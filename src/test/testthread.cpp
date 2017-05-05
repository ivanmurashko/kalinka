/**
   @file testthread.cpp
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
   - 2009/03/06 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "testthread.h"
#include "exception.h"
#include "testutils.h"

using namespace klk::test;

//
// Thread class
//

// Constructor
Thread::Thread() :
    base::Thread(), m_rc(klk::OK)
{
}

// Destructor
Thread::~Thread()
{
}

// Gets the result
void Thread::checkResult()
{
    klk::Locker lock(&m_lock);
    if (m_rc != OK)
    {
        throw Exception(__FILE__, __LINE__, "Thread::checkResult() failed");
    }
}

// Starts the thread
void Thread::start()
{
    try
    {
        mainLoop();
    }
    catch(const std::exception& err)
    {
        if (!isStopped())
        {
            printOut(std::string("\nError: ") + err.what() + "\n");
            klk::Locker lock(&m_lock);
            m_rc = ERROR;
        }
    }
}

//
// Scheduler class
//

// Constructor
Scheduler::Scheduler() :
    base::Scheduler(), m_testthreads()
{
}

// Destructor
Scheduler::~Scheduler()
{
}

// Adds a test thread
void Scheduler::addTestThread(const ThreadPtr& thread)
{
    BOOST_ASSERT(thread);
    m_testthreads.push_back(thread);
}

// Starts all threads
void Scheduler::start()
{
    std::for_each(m_testthreads.begin(), m_testthreads.end(),
                  boost::bind(&base::Scheduler::startThread,
                              this, _1));
}

// Checks the result
void Scheduler::checkResult()
{
    // check all producers
    std::for_each(m_testthreads.begin(), m_testthreads.end(),
                  boost::bind(&Thread::checkResult, _1));
}

// Clears the test thread list
void Scheduler::clear() throw()
{
    stop();
    m_testthreads.clear();
    base::Scheduler::clear();
}

