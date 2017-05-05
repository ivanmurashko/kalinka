/**
   @file testslowconnection.cpp
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
   - 2010/11/21 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "testslowconnection.h"
#include "testutils.h"
#include "defines.h"
#include "testdefines.h"
#include "binarydata.h"
#include "utils.h"
#include "socket/exception.h"

using namespace klk;
using namespace klk::http;

namespace
{
    const size_t DATASIZE = CONNECTIONBUFFER_MAX_SIZE/2 + 1;
}

//
// TestSlowConsumerThread class
//

// Constructor
TestSlowConsumerThread::TestSlowConsumerThread() :
    test::Thread()
{
}

// @copydoc klk::test::Thread::mainLoop
void TestSlowConsumerThread::mainLoop()
{
    ISocketPtr sock = sock::Factory::getSocket(sock::TCPIP);

    sock::RouteInfo
        route(TESTSERVERHOST, TESTSERVERPORT, sock::TCPIP, sock::UNICAST);
    sock->connect(route);
    BinaryData req("GET " + TESTPATH1 + " HTTP/1.0\r\n\r\n");
    sock->send(req);
    sleep(2 * WAITINTERVAL4SLOW + 2);
    try
    {
        BinaryData res;
        Result rc = sock->checkData(2);
        BOOST_ASSERT(rc == OK);
        BinaryData tmp(DATASIZE);
        sock->recv(tmp);
        klk_log(KLKLOG_DEBUG, "Got %d bytes at test thread",
                tmp.size());
        // at the recv we have to get less data than requested
        // because the connection was closed
        sock->recv(tmp);
        klk_log(KLKLOG_DEBUG, "Got %d bytes at test thread",
                tmp.size());
        BOOST_ASSERT(tmp.size() < DATASIZE);
    }
    catch(const ClosedConnection&)
    {
        // OK nothing to do
    }
}

//
// TestSlowProducerThread class
//

// Constructor
TestSlowProducerThread::TestSlowProducerThread() :
    TestProducerThread(sock::RouteInfo(TESTHOST1, TESTPORT1,
                                       sock::TCPIP, sock::UNICAST),
                       std::string())
{
    m_data.resize(DATASIZE);
    memset(m_data.toVoid(), 'A', m_data.size());
}

// @copydoc klk::test::Thread::mainLoop
void TestSlowProducerThread::mainLoop()
{
    klk_log(KLKLOG_DEBUG, "HTTP test: started \"slow\" producer thread. "
            "Host: %s. Port: %u.",
            m_route.getHost().c_str(),
            m_route.getPort());

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
// TestSlowConnection class
//

/// Constructor
TestSlowConnection::TestSlowConnection() :
    TestStreamer("testhttpstreamer_slow.sql")
{
}

// Test constructor
void TestSlowConnection::setUp()
{
    test::ThreadPtr thread(new TestSlowConsumerThread());
    m_scheduler.addTestThread(thread);
    m_consumers.push_back(thread);

    test::TestModuleWithDB::setUp();

    sleep(3);
}

/// The test itself
void TestSlowConnection::test()
{
    test::printOut("\nHTTP Streamer slow connections test ... ");

    test::ThreadPtr thread(new TestSlowProducerThread());
    m_scheduler.addTestThread(thread);

    // start consumer/producer threads
    m_scheduler.start();

    // wait for awhile
    sleep(3 * WAITINTERVAL4SLOW + 5);

    // stop all consumer threads first
    std::for_each(m_consumers.begin(), m_consumers.end(),
                  boost::bind(&base::Scheduler::stopThread,
                              &m_scheduler, _1));
    // stop all others
    m_scheduler.stop();

    // check result in consumers/producers
    m_scheduler.checkResult();
}
