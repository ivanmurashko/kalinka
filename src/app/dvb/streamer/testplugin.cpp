/**
   @file testplugin.cpp
   @brief This file is part of Kalinka mediaserver.
   @author ipp <ivan.murashko@gmail.com>

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
   - 2008/10/15 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

#include "testplugin.h"
#include "dev.h"
#include "testfactory.h"
#include "defines.h"
#include "testdefines.h"
#include "utils.h"
#include "paths.h"
#include "xml.h"
#include "dbscriptlauncher.h"
#include "testutils.h"

#include "./plugin/threadfactory.h"

// modules specific
#include "dvb/dvbdev.h"

using namespace klk;
using namespace klk::dvb::stream;

//
// TestStation class
//

// Constructor
TestStation::TestStation(const std::string& name,
                         const std::string& number,
                         const std::string& host,
                         const std::string& port) :
    m_name(name), m_number(number), m_host(host), m_port(port),
    m_rate(0)
{
    BOOST_ASSERT(m_name.empty() == false);
    BOOST_ASSERT(m_number.empty() == false);
    BOOST_ASSERT(m_host.empty() == false);
    BOOST_ASSERT(m_port.empty() == false);
}

// Retrives route info
const sock::RouteInfo TestStation::getRoute() const
{
    int port = 0;
    try
    {
        port = boost::lexical_cast<int>(m_port);

    }
    catch(const boost::bad_lexical_cast&)
    {
        throw Exception(__FILE__, __LINE__, err::BADLEXCAST);
    }

    return sock::RouteInfo(m_host, port, sock::UDP, sock::UNICAST);
}

// Gets the channel name
const std::string TestStation::getChannelName() const throw()
{
    return m_name;
}

// Gets the channel number
u_int TestStation::getChannelNumber() const
{
    u_int number = 0;
    try
    {
        number = boost::lexical_cast<int>(m_number);

    }
    catch(const boost::bad_lexical_cast&)
    {
        throw Exception(__FILE__, __LINE__, err::BADLEXCAST);
    }

    return number;
}

// This one retrives data rate for the station
const int TestStation::getRate() const throw()
{
    return m_rate.getValue();
}

// Sets rate
void TestStation::setRate(const int rate)
{
    BOOST_ASSERT(rate >= 0);
    m_rate = rate;
}


//
// TestPlugin class
//

// Constructor
TestPlugin::TestPlugin() : CppUnit::TestFixture(), dvb::TestHelper(),
                           m_dvbscheduler(test::Factory::instance()),
                           m_stations()
{
}

// Set up context before running a test.
void TestPlugin::setUp()
{
    // start only if we have a real device
    CPPUNIT_ASSERT(base::Utils::fileExist("/dev/dvb/adapter0") == 1);

    CPPUNIT_ASSERT(test::Factory::instance() != NULL);

    // reset test factory
    test::Factory::instance()->reset();

    // clear DB
    test::DBScriptLauncher launcher(test::Factory::instance()->getConfig());
    launcher.recreate();

    // set plugin as processor
    IFactory *factory = test::Factory::instance();
    IThreadFactoryPtr threadfactory(new getstream2::ThreadFactory(factory));
    m_dvbscheduler.setThreadFactory(threadfactory);

    // create dev
    createDev();

    // fillChannels
    fillChannels();
}

// Clean up after the test run.
void TestPlugin::tearDown()
{
    // clear resources
    CPPUNIT_ASSERT(test::Factory::instance());
    test::Factory::instance()->reset();
    test::Factory::instance()->getResources()->initDevs();
    // stop all activity left
    m_dvbscheduler.stop();

    // clear channels
    m_stations.clear();
}

// The unit test for OK requests
void TestPlugin::testOk()
{
    test::printOut("\nDVB Plugin test for correct tune ... ");

    startChannels();

    // sleep untill check will be done
    sleep(PLUGIN_UPDATEINFOINTERVAL + 50);

    // check dev state
    checkDev();

    // checks the channles
    checkChannels();

    // stop channels
    stopChannels();
}

/// structure to check a channel
struct CheckChannel : public std::unary_function<TestStationPtr, void>
{
    void operator()(TestStationPtr test_station){
        CPPUNIT_ASSERT(test_station);
        IStationPtr station = boost::dynamic_pointer_cast<IStation>(test_station);
        CPPUNIT_ASSERT(test_station);
        if (station->getRate() < 100000)
        {
            throw klk::Exception(__FILE__, __LINE__, "Station '%s' has invalid rate: %d",
                                 station->getChannelName().c_str(), station->getRate());
        }
    }
};

/// Checks channles for the data actually run on them
void TestPlugin::checkChannels()
{
    // more than one channel to stream should be here
    CPPUNIT_ASSERT(m_stations.size() > 1);

    std::for_each(m_stations.begin(), m_stations.end(), boost::bind(CheckChannel(), _1));
}

//  The unit test for wrong requests
void TestPlugin::testWrong()
{
    test::printOut("\nDVB Plugin test for incorrect tune ... ");

    // change frequency to wrong value
    getDev()->setParam(dev::FREQUENCY, "950001");

    startChannels(); // should produce exception
}

// Create dev for tests
void TestPlugin::createDev()
{
    // process the XML with data
    const std::string fname = dir::SHARE + "/test/testdvb.xml";
    TestHelper::createDev(fname);
}

// Checks dev state
void TestPlugin::checkDev()
{
    IDevPtr dev = getDev();
    CPPUNIT_ASSERT(dev);

    // CPPUNIT_ASSERT(dev->getState() == dev::WORK);
    //CPPUNIT_ASSERT(dev->getParam(dev::DVBACTIVITY) == dev::STREAMING);
    CPPUNIT_ASSERT(dev->getIntParam(dev::HASLOCK) == 1);
    //FIXME!!!CPPUNIT_ASSERT(dev->getParam(dev::SIGNAL) != "0");
}

// Fill channels list
void TestPlugin::fillChannels()
{
    m_stations.clear();

    const std::string fname = dir::SHARE + "/test/testdvb.xml";
    XML xml;
    xml.parseFromFile(fname);

    // add OK data
    for (u_int i = 1; i <= 1000; i++)
    {
        std::string tag_prefix = "/testdata/channel" +
            boost::lexical_cast<std::string>(i);

        try
        {
            xml.getValue(tag_prefix + "/channel_no");
        }
        catch(...)
        {
            break;
        }

        const std::string name = xml.getValue(tag_prefix + "/name");
        const std::string number = xml.getValue(tag_prefix + "/channel_no");
        const std::string host = xml.getValue(tag_prefix + "/host");
        const std::string port = xml.getValue(tag_prefix + "/port");

        TestStationPtr station(new TestStation(name, number, host, port));

        m_stations.push_back(station);
    }
}

// Starts channels
void TestPlugin::startChannels()
{
    m_dvbscheduler.createStreamThread(getDev());

    IStreamPtr stream = m_dvbscheduler.getStream(getDev());
    CPPUNIT_ASSERT(stream);

    for (TestStationList::iterator station = m_stations.begin();
         station != m_stations.end(); station++)
    {
        sleep(20);
        stream->addStation(*station);
    }

    sleep(PLUGIN_UPDATEINFOINTERVAL + 5);

    // check that we stream something
    CPPUNIT_ASSERT(stream->isStream());
}

// Starts channels
void TestPlugin::stopChannels()
{
    IStreamPtr stream = m_dvbscheduler.getStream(getDev());
    CPPUNIT_ASSERT(stream);

    for (TestStationList::iterator station = m_stations.begin();
         station != m_stations.end(); station++)
    {
        stream->delStation(*station);
        sleep(5);
    }

    sleep(PLUGIN_UPDATEINFOINTERVAL);

    // check that we dont stream something
    CPPUNIT_ASSERT(!stream->isStream());
}

