/**
   @file testudp.cpp
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
   - 2009/05/21 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "testudp.h"
#include "exception.h"
#include "testdefines.h"
#include "testhttpthread.h"
#include "streamer.h"
#include "defines.h"
#include "utils.h"
#include "testutils.h"

#include "socket/routeinfo.h"

using namespace klk;
using namespace klk::http;

//
// TestProducerThread4UDP class
//

// Constructor
TestProducerThread4UDP::TestProducerThread4UDP(const sock::RouteInfo& route,
                                               const std::string& data) :
    TestProducerThread(route, data)
{
    BOOST_ASSERT(m_route.getProtocol() == sock::UDP);
}

// Destructor
TestProducerThread4UDP::~TestProducerThread4UDP()
{
}

// Main thread body
void TestProducerThread4UDP::mainLoop()
{
    klk_log(KLKLOG_DEBUG, "HTTP test: started UDP producer thread. "
            "Host: %s. Port: %u. Data: %s.",
            m_route.getHost().c_str(),
            m_route.getPort(),
            m_data.toString().c_str());

    ISocketPtr sock = sock::Factory::getSocket(sock::UDP);

    while(!isStopped())
    {
        // make the send data
        // contains path and be unique
        BinaryData data(m_data);
        BinaryData uuid(base::Utils::generateUUID());
        data.add(uuid);
        // send our data
        sock->send(m_route, data);
        // sleep for awhile
        sleep(1);
    }
}


//
// TestUDP class
//

// Constructor
TestUDP::TestUDP() :
    TestStreamer("testhttpstreamer_udp.sql")
{
}

// Tests main loop
void TestUDP::test()
{
    test::printOut("\nHTTP Streamer UDP input connections test ... ");

    // create producers
    sock::RouteInfo
        route1(TESTHOST1, TESTPORT1, sock::UDP, sock::UNICAST),
        route2(TESTHOST2, TESTPORT2, sock::UDP, sock::UNICAST);
    test::ThreadPtr
        thread1(new TestProducerThread4UDP(route1, TESTPATH1));
    m_scheduler.addTestThread(thread1);
    test::ThreadPtr
        thread2(new TestProducerThread4UDP(route2, TESTPATH2));
    m_scheduler.addTestThread(thread2);


    // no read in input
    // before start consumers
    //test::printOut("\n\tCheck initial no read");
    checkNoRead();

    // start consumer/producer threads
    m_scheduler.start();

    // wait for awhile
    //test::printOut("\n\tMain loop ...");
    sleep(20);

    // stop all consumer threads first
    //test::printOut("\n\tStoping consumers ...");
    std::for_each(m_consumers.begin(), m_consumers.end(),
                  boost::bind(&base::Scheduler::stopThread,
                              &m_scheduler, _1));

    // sleep for awhile
    //test::printOut("\n\tWait after stop ...");
    sleep(10);

    // no read in input
    // after stop consumers
    //test::printOut("\n\tCheck final no read");
    checkNoRead();

    // stop all others
    //test::printOut("\n\tStop all test threads");
    m_scheduler.stop();

    // check result in consumers/producers
    //test::printOut("\n\tCheck result in test threads");
    m_scheduler.checkResult();
}

// Checks that there is no any read in input threads
void TestUDP::checkNoRead()
{
    // There are should be two input threads but both
    // should not be assosiated with any socket

    Factory* factory =
        getModule<Streamer>(MODID)->m_httpfactory;
    CPPUNIT_ASSERT(factory);

    // check input info
    CPPUNIT_ASSERT(factory->m_inthreads);
    InThreadContainer::InThreadList list = factory->m_inthreads->m_list;
    CPPUNIT_ASSERT(list.size() == 2);
    bool was1 = false, was2 = false;
    for (InThreadContainer::InThreadList::iterator i = list.begin();
         i != list.end(); i++)
    {
        const InputInfoPtr info = (*i)->getInfo();
        CPPUNIT_ASSERT(info);
        CPPUNIT_ASSERT(info->getMediaTypeUuid() == TESTMTYPEUUID);

        if (info->getPath() == TESTPATH1)
        {
            CPPUNIT_ASSERT(was1 == false);
            // reader should not be initialized
            CPPUNIT_ASSERT((*i)->m_reader == NULL);
            // listener too (not join to multicast)
            CPPUNIT_ASSERT((*i)->m_listener == NULL);
            was1 = true;
        }
        else if (info->getPath() == TESTPATH2)
        {
            CPPUNIT_ASSERT(was2 == false);
            // reader should not be initialized
            CPPUNIT_ASSERT((*i)->m_reader == NULL);
            // listener too (not join to multicast)
            CPPUNIT_ASSERT((*i)->m_listener == NULL);
            was2 = true;
        }
        else
        {
            CPPUNIT_ASSERT(0);
        }
    }
    CPPUNIT_ASSERT(was1);
    CPPUNIT_ASSERT(was2);
}
