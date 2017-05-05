/**
   @file dvbscheduler.cpp
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
   - 2009/06/04 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "dvbscheduler.h"
#include "exception.h"

#include "./plugin/threadfactory.h"

using namespace klk;
using namespace klk::dvb::stream;

//
// Scheduler class
//

// Constructor
Scheduler::Scheduler(IFactory* factory) :
    base::Scheduler(),
    m_factory(IThreadFactoryPtr(new getstream2::ThreadFactory(factory))),
    m_dvb_lock(), m_dvb_threadinfo()
{
}

// Destructor
Scheduler::~Scheduler()
{
}

// Sets thread factory
void Scheduler::setThreadFactory(const IThreadFactoryPtr& factory)
{
    BOOST_ASSERT(factory);
    m_factory.setValue(factory);
}

// Retrives factory
const IThreadFactoryPtr Scheduler::getThreadFactory() const
{
    IThreadFactoryPtr factory = m_factory.getValue();
    BOOST_ASSERT(factory);
    return factory;
}


// Creates a stream thread pointer
// The thread will be created and
// started if it was not created before
void Scheduler::createStreamThread(const IDevPtr& dev)
{
    BOOST_ASSERT(dev);
    Locker lock(&m_dvb_lock);
    if (!isStarted(dev))
    {
        IThreadInfoPtr threadinfo =
            getThreadFactory()->createStreamThread(dev);
        BOOST_ASSERT(threadinfo);
        startThread(threadinfo->getThread());
        m_dvb_threadinfo.push_back(threadinfo);
    }
}

/**
   Functor to find a thread by dev
*/
struct FindThreadByDev :
    public std::binary_function<IThreadInfoPtr, IDevPtr, bool>
{
    /**
       Finder itself
    */
    inline bool operator()(const IThreadInfoPtr info,
                           const IDevPtr dev)
        {
            BOOST_ASSERT(info);
            return (info->matchDev(dev));
        }
};

// Checks is any thread started for the current
// dev
// @note NOT THREAD SAFE CODE
bool Scheduler::isStarted(const IDevPtr& dev) const
{
    BOOST_ASSERT(dev);

    DVBThreadInfoList::const_iterator find =
        std::find_if(m_dvb_threadinfo.begin(),
                     m_dvb_threadinfo.end(),
                     boost::bind(FindThreadByDev(),_1, dev));
    return (find != m_dvb_threadinfo.end());
}


// Gets dvb thread for the specified dev
const IStreamPtr Scheduler::getStream(const IDevPtr& dev) const
{
    BOOST_ASSERT(dev);

    Locker lock(&m_dvb_lock);
    DVBThreadInfoList::const_iterator find =
        std::find_if(m_dvb_threadinfo.begin(),
                     m_dvb_threadinfo.end(),
                     boost::bind(FindThreadByDev(),_1, dev));
    if (find == m_dvb_threadinfo.end())
    {
        throw Exception(
            __FILE__, __LINE__,
            "No any DVB stream started for dev with uuid: '%s'",
            dev->getStringParam(dev::UUID).c_str());
    }

    return (*find)->getStream();
}

// Stops thread by dev
void Scheduler::stopThread(const IDevPtr& dev)
{
    BOOST_ASSERT(dev);

    Locker lock(&m_dvb_lock);
    DVBThreadInfoList::iterator find =
        std::find_if(m_dvb_threadinfo.begin(),
                     m_dvb_threadinfo.end(),
                     boost::bind(FindThreadByDev(),_1, dev));
    if (find != m_dvb_threadinfo.end())
    {
        IThreadPtr thread = (*find)->getThread();
        stopThread(thread);
    }
}

/**
   Functor to find a thread by dev
*/
struct FindThreadByThread :
    public std::binary_function<IThreadInfoPtr, IThreadPtr, bool>
{
    /**
       Finder itself
    */
    inline bool operator()(const IThreadInfoPtr info,
                           const IThreadPtr thread)
        {
            BOOST_ASSERT(info);
            return (info->getThread() == thread);
        }
};


// Stops a thread at the scheduler
void Scheduler::stopThread(const IThreadPtr& thread)
{
    base::Scheduler::stopThread(thread);

    // clear DVB thread info
    Locker lock(&m_dvb_lock);
    m_dvb_threadinfo.remove_if(boost::bind(FindThreadByThread(), _1, thread));
}

// Stops all threads
void Scheduler::stop() throw()
{
    base::Scheduler::stop();
    Locker lock(&m_dvb_lock);
    m_dvb_threadinfo.clear();
}

