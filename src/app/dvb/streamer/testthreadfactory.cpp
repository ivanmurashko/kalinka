/**
   @file testthreadfactory.cpp
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
   - 2009/06/05 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "testthreadfactory.h"
#include "exception.h"
#include "dvbthreadinfo.h"

// extrenal modules
#include "dvb/dvbdev.h"

using namespace klk;
using namespace klk::dvb::stream;

//
// TestThreadFactory class
//

// Constructor
TestThreadFactory::TestThreadFactory() :
    m_lock(), m_stations()
{
}

// Destructor
TestThreadFactory::~TestThreadFactory()
{
}

// Checks is there any streaming channels started
bool TestThreadFactory::empty() const
{
    Locker lock(&m_lock);
    return m_stations.empty();
}

// Checks is there a channel with the specified name
bool TestThreadFactory::isStream(const std::string& channel)
{
    Locker lock(&m_lock);
    IStationList::iterator find =
        std::find_if(m_stations.begin(), m_stations.end(),
                     boost::bind(
                         std::equal_to<std::string>(),
                         channel,
                         boost::bind(&IStation::getChannelName, _1)));
    return (find != m_stations.end());
}

// Adds a station
void TestThreadFactory::addStation(const IStationPtr& station)
{
    BOOST_ASSERT(station);
    // check that no such station yet
    KLKASSERT(isStream(station->getChannelName()) == false);
    Locker lock(&m_lock);
    m_stations.push_back(station);
}

// Dels a station
void TestThreadFactory::delStation(const IStationPtr& station)
{
    BOOST_ASSERT(station);
    BOOST_ASSERT(isStream(station->getChannelName()) == true);
    Locker lock(&m_lock);
    m_stations.remove_if(boost::bind(
                             std::equal_to<std::string>(),
                             station->getChannelName(),
                             boost::bind(&IStation::getChannelName, _1)));
    BOOST_ASSERT(isStream(station->getChannelName()) == false);
    klk_log(KLKLOG_DEBUG, "Remove DVB channel: %s",
            station->getChannelName().c_str());
}

// Gets a StreamThread pointer
const IThreadInfoPtr TestThreadFactory::createStreamThread(const IDevPtr& dev)
{
    boost::shared_ptr<TestThread>
        thread(new TestThread(this));
    // the dev always has a lock
    dev->setParam(dev::HASLOCK, 1);
    return IThreadInfoPtr(new ThreadInfo(thread, dev, thread));
}

//
// TestThread class
//

// Constructor
TestThread::TestThread(TestThreadFactory* factory) :
    base::Thread(), m_factory(factory), m_stations()
{
    BOOST_ASSERT(m_factory);
}

// Destructor
TestThread::~TestThread()
{
}

// starts the thread
void TestThread::start()
{
    // nothing to do
}

// stops stream thread
void TestThread::stop() throw()
{
    klk_log(KLKLOG_ERROR, "Test thread got stop signal. "
            "Stations to stream: %d", m_stations.size());

    base::Thread::stop();
    // clear the stations
    std::for_each(
        m_stations.begin(), m_stations.end(),
        boost::bind(&TestThreadFactory::delStation, m_factory, _1));
    m_stations.clear();
}

// Adds a channel
void TestThread::addStation(const IStationPtr& station)
{
    m_factory->addStation(station);
    Locker lock(&m_lock);
    m_stations.push_back(station);
}

// Dels a channel
void TestThread::delStation(const IStationPtr& station)
{
    m_factory->delStation(station);
    Locker lock(&m_lock);
    m_stations.remove_if(boost::bind(
                             std::equal_to<std::string>(),
                             station->getChannelName(),
                             boost::bind(&IStation::getChannelName, _1)));
}

// Checks is there any striming activity at the streaming thread
const bool TestThread::isStream() const
{
    Locker lock(&m_lock);
    return (!m_stations.empty());
}

