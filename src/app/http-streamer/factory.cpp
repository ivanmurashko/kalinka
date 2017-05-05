/**
   @file factory.cpp
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
   - 2009/03/21 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "httpfactory.h"
#include "txtreader.h"
#include "flvreader.h"
#include "mpegtsreader.h"
#include "theora.h"
#include "defines.h"
#include "intcp.h"
#include "inudp.h"

#include "media.h"
#include "exception.h"

using namespace klk;
using namespace klk::http;

//
// Factory class
//

// Constructor
Factory::Factory() :
    m_lock(),
    m_stop(),
    m_scheduler(),
    m_stopthread(),
    m_outthread(),
    m_inthreads(),
    m_conthreads(),
    m_readers()
{
    m_readers[media::TXT] = boost::bind(&TXTReader::make, _1);
    m_readers[media::FLV] = boost::bind(&FLVReader::make, _1);
    m_readers[media::MPEGTS] = boost::bind(&MPEGTSReader::make, _1);
    m_readers[media::THEORA] = boost::bind(&TheoraReader::make, _1);
}

// Destructor
Factory::~Factory()
{
    clear();
}

// Inits factory usage
void Factory::init()
{
    try
    {
        m_stop.init();
        if (m_scheduler)
        {
            m_scheduler->stop();
        }
        m_scheduler.reset();
        m_scheduler = SchedulerPtr(new base::Scheduler());
        m_outthread.reset();
        m_outthread = OutThreadPtr(new OutThread(this));
        m_stopthread.reset();
        m_stopthread = StopThreadPtr(new StopThread(this));
        m_inthreads.reset();
        m_inthreads = InThreadContainerPtr(new InThreadContainer(this));
        m_conthreads.reset();
        m_conthreads =
            ConnectThreadContainerPtr(new ConnectThreadContainer(this));
    }
    catch(const std::bad_alloc&)
    {
        throw Exception(__FILE__, __LINE__, err::MEMORYALLOC);
    }
    BOOST_ASSERT(m_scheduler);

    // start threads
    m_scheduler->startThread(m_stopthread);
}

// Stops factory usage
void Factory::stop() throw()
{
    klk_log(KLKLOG_DEBUG, "Stopping HTTP streamer");
    m_stop.stop();
    if (m_inthreads)
    {
        m_inthreads->stop();
    }
    if (m_conthreads)
    {
        m_conthreads->stop();
    }

    if (m_scheduler)
    {
        m_scheduler->stop();
    }

    clear();
}

// Clears all internal data
void Factory::clear() throw()
{
    m_inthreads.reset();
    m_conthreads.reset();
    m_scheduler.reset();
    m_outthread.reset();
    m_stopthread.reset();
}

// Retrives pointer to scheduler
const SchedulerPtr Factory::getScheduler() const
{
    Locker lock(&m_lock);
    BOOST_ASSERT(m_scheduler);
    return m_scheduler;
}

// Setups out rout info
void Factory::setOutRoute(const RouteInfoPtr& route)
{
    if (m_stop.isStopped())
    {
        return;
    }

    getOutThread()->setRoute(route);

    // restart the out route
    if (getScheduler()->isStarted(m_outthread))
    {
        getScheduler()->stopThread(m_outthread);
    }
    getScheduler()->startThread(m_outthread);
}

// Retrives ouput thread
const OutThreadPtr Factory::getOutThread() const
{
    Locker lock(&m_lock);
    BOOST_ASSERT(m_outthread);
    return m_outthread;
}

// Retrives ouput thread
const StopThreadPtr Factory::getStopThread() const
{
    Locker lock(&m_lock);
    BOOST_ASSERT(m_stopthread);
    return m_stopthread;
}

// Retrives input thread container
const InThreadContainerPtr Factory::getInThreadContainer() const
{
    Locker lock(&m_lock);
    BOOST_ASSERT(m_inthreads);
    return m_inthreads;
}

// Retrives connection thread container
const ConnectThreadContainerPtr Factory::getConnectThreadContainer() const
{
    Locker lock(&m_lock);
    BOOST_ASSERT(m_conthreads);
    return m_conthreads;
}

// Gets reader by the media type (HTTP) uuid
const IReaderPtr Factory::getReader(const std::string& media_type,
                                    const ISocketPtr& sock) const
{
    BOOST_ASSERT(media_type.empty() == false);
    BOOST_ASSERT(sock);

    MakeReaderStorage::const_iterator find = m_readers.find(media_type);

    if (find == m_readers.end())
    {
        throw Exception(__FILE__, __LINE__,
                        "Unsupported media type for HTTP streamer: " +
                        media_type);
    }

    return (find->second)(sock);
}

// Created an input thread from input info
const InThreadPtr Factory::getInThread(const InputInfoPtr& info)
{
    InThreadPtr thread;

    BOOST_ASSERT(info);
    BOOST_ASSERT(info->getRouteInfo()->getHost().empty() == false);

    try
    {
        switch (info->getRouteInfo()->getProtocol())
        {
        case sock::TCPIP:
            thread = InThreadPtr(new InTCPThread(this, info));
            break;
        case sock::UDP:
            thread = InThreadPtr(new InUDPThread(this, info));
            break;
        default:
            throw Exception(__FILE__, __LINE__,
                            "Unsupported net protocol "
                            "for HTTP streamer: %d",
                            info->getRouteInfo()->getProtocol());
        }
    }
    catch(const std::bad_alloc&)
    {
        throw Exception(__FILE__, __LINE__, err::MEMORYALLOC);
    }

    BOOST_ASSERT(thread);

    return thread;
}
