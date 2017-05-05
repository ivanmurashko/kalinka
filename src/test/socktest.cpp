/**
   @file socktest.cpp
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
   - 2009/03/07 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "socktest.h"
#include "exception.h"
#include "utils.h"
#include "socket/exception.h"
#include "testutils.h"

using namespace klk;
using namespace klk::test;

/**
   Test socket host
*/
const std::string TESTSOCK_HOST = "localhost";

/**
   Test socket file
*/
const std::string TESTSOCK_FILE = "/tmp/testdomainsock";


/**
   Test socket port
*/
const u_int TESTSOCK_PORT = 80600;

/**
   Test buffer size
*/
const size_t SOCKBUFFSIZE = (13*1024);

//
// We always send data size at the begining of send request
// to be able read all necessary in corresponding recieve
// Darwin test has some trouble with it
//

namespace
{
    /**
       used to send a data protion

       @param[in] sock - the socket to be used
       @param[in] data - the data to be sent
    */
    void sendData(const ISocketPtr& sock, const BinaryData& data)
    {
        size_t size = data.size();
        if (size > 0)
        {
            BOOST_ASSERT(sock);

            // send data size first
            BinaryData size_data(sizeof(size));
            memcpy(size_data.toVoid(), &size, size_data.size());
            sock->send(size_data);

            // send main data
            sock->send(data);
        }
    }

    /**
       used to send a data protion

       @param[in] sock - the socket to be used
       @param[out] data - the data to be sent
    */
    void recvData(const ISocketPtr& sock, BinaryData& data)
    {
        BOOST_ASSERT(sock);
        // recv data size first
        size_t size = 0;
        BinaryData size_data(sizeof(size));
        sock->recvAll(size_data);
        memcpy(&size, size_data.toVoid(), sizeof(size));
        // recv all data
        data.resize(size);
        sock->recvAll(data);
    }
}


//
// TCPConnectThread class
//

// Constructor
TCPConnectThread::TCPConnectThread() :
    m_sock(sock::Factory::getSocket(sock::TCPIP))
{
}

// Destructor
TCPConnectThread::~TCPConnectThread()
{
}

// The main body of the thread
void TCPConnectThread::mainLoop()
{
    sock::RouteInfo testroute(TESTSOCK_HOST, TESTSOCK_PORT,
                              sock::TCPIP, sock::UNICAST);
    // first step is to check connection
    // we don't have it during 5 seconds
    Result rc = sock::Factory::checkConnection(testroute);
    BOOST_ASSERT(rc == ERROR);
    sleep(5);
    // now we hould have it
    rc = sock::Factory::checkConnection(testroute);
    BOOST_ASSERT(rc == OK);

    // try several times
    for (int i = 0; i < 10; i++)
    {
        // do the connection
        m_sock->connect(testroute);
        Result rc = m_sock->checkData(0);
        BOOST_ASSERT(rc == ERROR);

        BinaryData req;
        // send the binary data
        // read random data for 2.5 socket buffer
        req = base::Utils::readDataFromFile(
            "/dev/urandom",
            SOCKBUFFSIZE*5/2);
        BOOST_ASSERT(req.size() == SOCKBUFFSIZE*5/2);
        sendData(m_sock, req);

        BinaryData res;
        while (m_sock->checkData(1) == OK)
        {
            BinaryData buff(SOCKBUFFSIZE);
            recvData(m_sock, buff);
            if (buff.empty())
                break;
            res.add(buff);
        }
        if (req != res)
        {
            throw Exception(
                __FILE__, __LINE__,
                "Failed socket test at attempt number %d",
                i);
        }

        m_sock->disconnect();
    }

    // Final test for Socket::stopCheckData()
    m_sock->connect(testroute);
    rc = m_sock->checkData(30);
    BOOST_ASSERT(rc == ERROR);

    m_sock->disconnect();
}

// stops the thread
void TCPConnectThread::stop() throw()
{
    Thread::stop();
    m_sock->stopCheckData();
}

//
// DomainConnectThread class
//

// Constructor
DomainConnectThread::DomainConnectThread() :
    m_sock(sock::Factory::getSocket(sock::UNIXDOMAIN))
{
}

// Destructor
DomainConnectThread::~DomainConnectThread()
{
}

// The main body of the thread
void DomainConnectThread::mainLoop()
{
    CPPUNIT_ASSERT(!base::Utils::fileExist(TESTSOCK_FILE));

    sock::RouteInfo testroute(TESTSOCK_FILE, 0,
                              sock::UNIXDOMAIN, sock::UNICAST);
    // first step is to check connection
    // we don't have it during 5 seconds
    Result rc = sock::Factory::checkConnection(testroute);
    BOOST_ASSERT(rc == ERROR);
    sleep(5);
    // now we hould have it
    rc = sock::Factory::checkConnection(testroute);
    BOOST_ASSERT(rc == OK);

    // check that we have a file
    CPPUNIT_ASSERT(base::Utils::fileExist(TESTSOCK_FILE));

    // try several times
    for (int i = 0; i < 10; i++)
    {
        // do the connection
        m_sock->connect(testroute);
        Result rc = m_sock->checkData(0);
        BOOST_ASSERT(rc == ERROR);

        BinaryData req;
        // send the binary data
        // read random data for 2.5 socket buffer
        req = base::Utils::readDataFromFile(
            "/dev/urandom",
            SOCKBUFFSIZE*5/2);
        BOOST_ASSERT(req.size() == SOCKBUFFSIZE*5/2);
        sendData(m_sock, req);

        BinaryData res;
        while (m_sock->checkData(1) == OK)
        {
            BinaryData buff(SOCKBUFFSIZE);
            recvData(m_sock, buff);
            if (buff.empty())
                break;
            res.add(buff);
        }
        if (req != res)
        {
            throw Exception(
                __FILE__, __LINE__,
                "Failed socket test at attempt number %d",
                i);
        }

        m_sock->disconnect();
    }

    // Final test for Socket::stopCheckData()
    m_sock->connect(testroute);
    rc = m_sock->checkData(30);
    BOOST_ASSERT(rc == ERROR);
    m_sock->disconnect();
}

// stops the thread
void DomainConnectThread::stop() throw()
{
    Thread::stop();
    m_sock->stopCheckData();
}


//
// UDPConnectThread
//

// Constructor
UDPConnectThread::UDPConnectThread()
{
}

// Destructor
UDPConnectThread::~UDPConnectThread()
{
}

// The main body of the thread
void UDPConnectThread::mainLoop()
{
    sock::RouteInfo testroute(TESTSOCK_HOST, TESTSOCK_PORT,
                              sock::UDP, sock::UNICAST);
    // just start it
    sleep(4);

    // try several times
    for (int i = 0; i < 5; i++)
    {
        ISocketPtr sock = sock::Factory::getSocket(sock::UDP);
        // do the connection
        sock->connect(testroute);
        Result rc = sock->checkData(0);
        BOOST_ASSERT(rc == ERROR);
        for (int j = 0; j < 3; j++)
        {
            BinaryData req = base::Utils::readDataFromFile(
                "/dev/urandom",
                SOCKBUFFSIZE);
            if (j == 2)
                req.resize(SOCKBUFFSIZE);
            sendData(sock, req);
            BinaryData res(SOCKBUFFSIZE);
            recvData(sock, res);
            if (req != res)
            {
                throw Exception(
                    __FILE__, __LINE__,
                    "Failed socket test at attempt number %d:%d",
                    i, j);
            }
        }
    }
}


// stops the thread
void UDPConnectThread::stop() throw()
{
    Thread::stop();
}


//
// ListenThread class
//

// Constructor
ListenThread::ListenThread(const sock::Protocol proto) :
    m_proto(proto), m_listener()
{
    BOOST_ASSERT(m_proto == sock::TCPIP ||
                  m_proto == sock::UDP || m_proto == sock::UNIXDOMAIN);
}

// Destructor
ListenThread::~ListenThread()
{
}

// The main body of the thread
void ListenThread::mainLoop()
{
    BOOST_ASSERT(m_listener == NULL);
    int port = 0;
    std::string host = TESTSOCK_FILE;
    if (m_proto != sock::UNIXDOMAIN)
    {
        port = TESTSOCK_PORT;
        host = TESTSOCK_HOST;
    }
    sock::RouteInfo testroute(host, port,
                              m_proto, sock::UNICAST);
    // sleep before start
    sleep(3);

    m_listener = sock::Factory::getListener(testroute);

    // work in the echo mode
    while (!isStopped())
    {
        ISocketPtr sock = m_listener->accept();
        if (isStopped())
            break;
        BOOST_ASSERT(sock);
        if (m_proto == sock::TCPIP)
        {
            // check peername
            const std::string peername = sock->getPeerName();
            BOOST_ASSERT(peername == "127.0.0.1");
        }
        klk_log(KLKLOG_DEBUG,
                "Listen thread got a connection");
        if (!isStopped())
        {
            while (sock->checkData(40) == OK && !isStopped())
            {
                klk_log(KLKLOG_DEBUG,
                        "Listen thread is processing a data portion");
                BinaryData data(SOCKBUFFSIZE);
                BOOST_ASSERT(data.empty() == false);
                try
                {
                    recvData(sock, data);
                }
                catch(const ClosedConnection&)
                {
                    // check connection?
                    break;
                }
                sendData(sock, data);
            }
            sock->disconnect();
        }
    }
}

// The stop
void ListenThread::stop() throw()
{
    test::Thread::stop();
    if (m_listener)
    {
        m_listener->stop();
    }
}

//
// SocketTest class
//

// Constructor
SocketTest::SocketTest() :
    m_scheduler()
{
}

// Destructor
SocketTest::~SocketTest()
{
}

// Setups data
void SocketTest::setUp()
{
    m_scheduler.clear();
    try
    {
        base::Utils::unlink(TESTSOCK_FILE);
    }
    catch(...)
    {
        // ignore unlink exceptions
    }
}

// Clears data
void SocketTest::tearDown()
{
    m_scheduler.stop();
    m_scheduler.clear();
}

// Main test
void SocketTest::test(const sock::Protocol proto)
{
    CPPUNIT_ASSERT(proto == sock::TCPIP || proto == sock::UDP ||
                   proto == sock::UNIXDOMAIN);

    ThreadPtr thread1(new ListenThread(proto));
    m_scheduler.addTestThread(thread1);
    switch (proto)
    {
    case sock::TCPIP:
    {
        ThreadPtr thread2(new TCPConnectThread());
        m_scheduler.addTestThread(thread2);
        break;
    }
    case sock::UDP:
    {
        ThreadPtr thread2(new UDPConnectThread());
        m_scheduler.addTestThread(thread2);
        break;
    }
    case sock::UNIXDOMAIN:
    {
        ThreadPtr thread2(new DomainConnectThread());
        m_scheduler.addTestThread(thread2);
        break;
    }
    default:
        CPPUNIT_ASSERT(0);
    }


    time_t start = time(NULL);
    m_scheduler.start();
    sleep(20);
    m_scheduler.stop();
    time_t stop = time(NULL);

    CPPUNIT_NS::stdCOut() << "\nExecution time: " << stop - start;
    CPPUNIT_NS::stdCOut().flush();

    // Check time ( Socket::stopCheckData())
    CPPUNIT_ASSERT(stop - start < 25);

    m_scheduler.checkResult();
}


// Tests the TCP/IP sockets
void SocketTest::testTCPIP()
{
    printOut("\nTCP/IP Socket test ... ");

    test(sock::TCPIP);
}

// Tests the UDP sockets
void SocketTest::testUDP()
{
    printOut("\nUDP Socket test ... ");

    test(sock::UDP);
}

// Tests the UDP sockets
void SocketTest::testDomain()
{
    printOut("\nDomain Socket test ... ");

    test(sock::UNIXDOMAIN);
}


// Tests the exception after two connect
void SocketTest::test2Connect()
{
    printOut("\nSocket (TCP/IP) exception test ... ");

    ThreadPtr thread(new ListenThread(sock::TCPIP));
    m_scheduler.addTestThread(thread);

    m_scheduler.start();
    sleep(5);

    // Test connection 2 times
    ISocketPtr sock = sock::Factory::getSocket(sock::TCPIP);
    sock::RouteInfo testroute(TESTSOCK_HOST, TESTSOCK_PORT,
                              sock::TCPIP, sock::UNICAST);
    Result rc = sock::Factory::checkConnection(testroute);
    CPPUNIT_ASSERT(rc == OK);
    sock->connect(testroute);
    sock->connect(testroute); // should produce exception ???
    m_scheduler.stop();
}
