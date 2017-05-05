/**
   @file testtcp.cpp
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

#include "testtcp.h"
#include "exception.h"
#include "testdefines.h"
#include "testhttpthread.h"

#include "socket/routeinfo.h"
#include "utils.h"
#include "testutils.h"

using namespace klk;
using namespace klk::http;

//
// TestProducerThread4TCP class
//

// Constructor
TestProducerThread4TCP::TestProducerThread4TCP(const sock::RouteInfo& route,
                                               const std::string& data) :
    TestProducerThread(route, data)
{
    BOOST_ASSERT(m_route.getProtocol() == sock::TCPIP);
}

// Destructor
TestProducerThread4TCP::~TestProducerThread4TCP()
{
}

// Starts the thread
void TestProducerThread4TCP::mainLoop()
{
    klk_log(KLKLOG_DEBUG, "HTTP test: started TCP/IP producer thread. "
            "Host: %s. Port: %u. Data: %s.",
            m_route.getHost().c_str(),
            m_route.getPort(),
            m_data.toString().c_str());

    ISocketPtr sock = sock::Factory::getSocket(m_route.getProtocol());

    sock->connect(m_route);
    while(!isStopped())
    {
        // make the send data
        // contains path and be unique
        BinaryData data(m_data);
        BinaryData uuid(base::Utils::generateUUID());
        data.add(uuid);
        // send our data
        sock->send(data);

        sleep(1);
    }
}

//
// TestTCP class
//

// Constructor
TestTCP::TestTCP() :
    TestStreamer("testhttpstreamer_tcpip.sql")
{
}

// Tests main loop
void TestTCP::test()
{
    test::printOut("\nHTTP Streamer TCP/IP input connections test ... ");

    // create producers
    sock::RouteInfo
        route1(TESTHOST1, TESTPORT1, sock::TCPIP, sock::UNICAST),
        route2(TESTHOST2, TESTPORT2, sock::TCPIP, sock::UNICAST);
    test::ThreadPtr
        thread1(new TestProducerThread4TCP(route1, TESTPATH1));
    m_scheduler.addTestThread(thread1);
    test::ThreadPtr
        thread2(new TestProducerThread4TCP(route2, TESTPATH2));
    m_scheduler.addTestThread(thread2);

    // start consumer/producer threads
    m_scheduler.start();

    // wait for awhile
    sleep(20);

    // stop all consumer threads first
    std::for_each(m_consumers.begin(), m_consumers.end(),
                  boost::bind(&base::Scheduler::stopThread,
                              &m_scheduler, _1));
    // stop all others
    m_scheduler.stop();

    // check result in consumers/producers
    m_scheduler.checkResult();
}

