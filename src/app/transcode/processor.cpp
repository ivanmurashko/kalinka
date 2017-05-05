/**
   @file processor.cpp
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
   - 2009/11/05 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "processor.h"
#include "exception.h"
#include "defines.h"

using namespace klk;
using namespace klk::trans;

//
// ProcessorThread class
//

// Constructor
ProcessorThread::ProcessorThread(Scheduler* scheduler) :
    base::Thread(), m_processors(), m_storage(), m_event()
{
    BOOST_ASSERT(scheduler);
    m_processors[gst::ERROR] = boost::bind(&Scheduler::processStopEvent,
                                           scheduler, _1);
    m_processors[gst::EOS] = boost::bind(&Scheduler::processStopEvent,
                                         scheduler, _1);
}

// Destructor
ProcessorThread::~ProcessorThread()
{
}

// Inits the thread
void ProcessorThread::init()
{
    base::Thread::init();
    Locker lock(&m_lock);
    m_storage.clear();
}

// Starts the thread
void ProcessorThread::start()
{
    while (!isStopped())
    {
        try
        {
            processEvents();
        }
        catch(...)
        {
            klk_log(KLKLOG_ERROR, "There was an error while processing "
                    "a GST event");
        }
        m_event.startWait(SCHEDULE_INTERVAL/*FIXME!!!*/);
    }
}

// Processes all events
void ProcessorThread::processEvents()
{
    while (gst::IEventPtr event = getEvent())
    {
        // looking for event processor
        Processors::iterator find = m_processors.find(event->getType());
        if (find == m_processors.end())
        {
            klk_log(KLKLOG_ERROR, "Unsupported GST event with type: %d",
                    event->getType());
            continue;
        }
        // processing the event
        find->second(event);
    }
}

// Stops the thread
void ProcessorThread::stop() throw()
{
    base::Thread::stop();
    m_event.stopWait();
}

// Adds an event for processing
void ProcessorThread::addEvent(const gst::IEventPtr& event)
{
    Locker lock(&m_lock);
    m_storage.push_back(event);
    // send notification
    m_event.stopWait();
}

// Retrives an event from the storage
const gst::IEventPtr ProcessorThread::getEvent()
{
    gst::IEventPtr event;
    Locker lock(&m_lock);
    if (!m_storage.empty())
    {
        event = m_storage.front();
        m_storage.pop_front();
    }
    return event;
}
