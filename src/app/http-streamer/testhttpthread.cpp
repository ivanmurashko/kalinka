/**
   @file testhttpthread.cpp
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

#include <cppunit/extensions/HelperMacros.h>

#include "testhttpthread.h"
#include "exception.h"
#include "testdefines.h"
#include "testutils.h"

// modules specific
#include "network/defines.h"

using namespace klk;
using namespace klk::http;

//
// TestProducerThread class
//

// Constructor
TestProducerThread::TestProducerThread(const sock::RouteInfo& route,
                                       const std::string& data) :
    test::Thread(), m_route(route),
    m_data(data)
{
}

// Destructor
TestProducerThread::~TestProducerThread()
{
}


//
// TestConsumerThread class
//

// Constructor
TestConsumerThread::TestConsumerThread() :
    test::Thread()
{
}

// Destructor
TestConsumerThread::~TestConsumerThread()
{
}

// Starts the thread
void TestConsumerThread::mainLoop()
{
    while (!isStopped())
    {
        sleep(1);
        testHead(TESTPATH1);
        testGet(TESTPATH1);
        testHead(TESTPATH2);
        testGet(TESTPATH2);
        testNotFound();
    }
}

// Test head request/response
// Trying 127.0.0.1...
// Connected to localhost.
// Escape character is '^]'.
// HEAD / HTTP/1.0
//
// HTTP/1.1 200 OK
// Date: Fri, 06 Mar 2009 13:52:01 GMT
// Server: Apache/2.2.9 (Ubuntu) PHP/5.2.6-2ubuntu4.1 with Suhosin-Patch
// Last-Modified: Mon, 01 Sep 2008 08:00:49 GMT
// ETag: "7b509-2d-455d10055ee40"
// Accept-Ranges: bytes
// Content-Length: 45
// Connection: close
// Content-Type: text/html
// Connection closed by foreign host.
void TestConsumerThread::testHead(const std::string& path)
{
    if (isStopped())
        return;

    ISocketPtr sock = sock::Factory::getSocket(sock::TCPIP);

    sock::RouteInfo
        route(TESTSERVERHOST, TESTSERVERPORT, sock::TCPIP, sock::UNICAST);
    sock->connect(route);
    BinaryData req("HEAD " + path + " HTTP/1.0\r\n\r\n");
    sock->send(req);
    BinaryData res;
    BinaryData tmp(SOCKBUFFSIZE);
    Result rc = sock->checkData(1);
    BOOST_ASSERT(rc == OK);
    while (tmp.size() == SOCKBUFFSIZE)
    {
        sock->recv(tmp);
        res.add(tmp);
    }

    // test the response
    const std::string response = res.toString();
    klk_log(KLKLOG_DEBUG, "TestConsumerThread::testHead() got response: %s",
            response.c_str());
    BOOST_ASSERT(response.empty() == false);
    BOOST_ASSERT(response.find("HTTP/1.1 200 OK") != std::string::npos);
    BOOST_ASSERT(response.find("Content-Type: ") != std::string::npos);
    // should not find path here
    BOOST_ASSERT(response.find(path) == std::string::npos);
}


// Test get request/response
// GET / HTTP/1.0
//
// HTTP/1.1 200 OK
// Date: Fri, 06 Mar 2009 13:55:21 GMT
// Server: Apache/2.2.9 (Ubuntu) PHP/5.2.6-2ubuntu4.1 with Suhosin-Patch
// Last-Modified: Mon, 01 Sep 2008 08:00:49 GMT
// ETag: "7b509-2d-455d10055ee40"
// Accept-Ranges: bytes
// Content-Length: 45
// Connection: close
// Content-Type: text/html
//
// <html><body><h1>It works!</h1></body></html>
void TestConsumerThread::testGet(const std::string& path)
{
    if (isStopped())
        return;

    ISocketPtr sock = sock::Factory::getSocket(sock::TCPIP);

    sock::RouteInfo
        route(TESTSERVERHOST, TESTSERVERPORT, sock::TCPIP, sock::UNICAST);
    sock->connect(route);
    BinaryData req("GET " + path + " HTTP/1.0\r\n\r\n");
    sock->send(req);
    BinaryData res;
    BinaryData tmp(SOCKBUFFSIZE);
    Result rc = sock->checkData(2);
    BOOST_ASSERT(rc == OK);
    while (tmp.size() == SOCKBUFFSIZE)
    {
        sock->recv(tmp);
        res.add(tmp);
    }
    // test the response
    const std::string response = res.toString();
    BOOST_ASSERT(response.empty() == false);
    // test the response
    BOOST_ASSERT(response.find("HTTP/1.1 200 OK") != std::string::npos);
    BOOST_ASSERT(response.find("Content-Type: ") != std::string::npos);
    //BOOST_ASSERT(response.find(path) == std::string::npos);

    // we receive path every 1 second
    BinaryData path1(SOCKBUFFSIZE);
    BinaryData path2(SOCKBUFFSIZE);
    rc = sock->checkData(2);
    BOOST_ASSERT(rc == OK);
    sock->recv(path1);
    BOOST_ASSERT(path1.toString().find(path) != std::string::npos);
    rc = sock->checkData(2);
    BOOST_ASSERT(rc == OK);
    sock->recv(path2);
    BOOST_ASSERT(path2.toString().find(path) != std::string::npos);
    BOOST_ASSERT(path1.toString() != path2.toString());
}

// Test 404 http error
// HEAD /blabla HTTP/1.0
//
// HTTP/1.1 404 Not Found
// Date: Sun, 08 Mar 2009 17:37:58 GMT
// Server: Apache/2.2.11 (Debian) PHP/5.2.6-3 ...
// Connection: close
// Content-Type: text/html; charset=iso-8859-1
//
// GET /blabla HTTP/1.0
//
// HTTP/1.1 404 Not Found
// Date: Sun, 08 Mar 2009 17:41:31 GMT
// Server: Apache/2.2.11 (Debian) PHP/5.2.6-3 ...
// Content-Length: 367
// Connection: close
// Content-Type: text/html; charset=iso-8859-1
//
// <!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 2.0//EN">
// <html><head>
// <title>404 Not Found</title>
// </head><body>
// <h1>Not Found</h1>
// <p>The requested URL /blabla was not found on this server.</p>
// <hr>
// <address>Apache/2.2.11 (Debian) PHP/5.2.6-3 .. </address>
// </body></html>
// Connection closed by foreign host.
void TestConsumerThread::testNotFound()
{
    if (isStopped())
        return;

    sock::RouteInfo
        route(TESTSERVERHOST, TESTSERVERPORT, sock::TCPIP, sock::UNICAST);
    {
        ISocketPtr sock = sock::Factory::getSocket(sock::TCPIP);
        sock->connect(route);
        BinaryData req("HEAD /blabla HTTP/1.1\r\n\r\n");
        sock->send(req);
        Result rc = sock->checkData(2);
        BOOST_ASSERT(rc == OK);
        BinaryData res;
        BinaryData tmp(SOCKBUFFSIZE);
        while (tmp.size() == SOCKBUFFSIZE)
        {
            sock->recv(tmp);
            res.add(tmp);
        }
        // test the response
        const std::string response = res.toString();
        BOOST_ASSERT(response.empty() == false);
        // test the response
        BOOST_ASSERT(response.find("HTTP/1.1 404 Not Found") !=
                     std::string::npos);
        BOOST_ASSERT(response.find("Content-Type: ") == std::string::npos);
        BOOST_ASSERT(response.find(TESTPATH1) == std::string::npos);
        BOOST_ASSERT(response.find(TESTPATH2) == std::string::npos);
    }

    {
        ISocketPtr sock = sock::Factory::getSocket(sock::TCPIP);

        sock->connect(route);
        BinaryData req("GET /blabla HTTP/1.0\r\n\r\n");
        sock->send(req);
        Result rc = sock->checkData(2);
        BOOST_ASSERT(rc == OK);
        BinaryData res;
        BinaryData tmp(SOCKBUFFSIZE);
        while (tmp.size() == SOCKBUFFSIZE)
        {
            sock->recv(tmp);
            res.add(tmp);
        }
        // test the response
        const std::string response = res.toString();
        BOOST_ASSERT(response.empty() == false);
        // test the response
        BOOST_ASSERT(response.find("HTTP/1.1 404 Not Found") !=
                     std::string::npos);
        BOOST_ASSERT(response.find("Content-Type: ") == std::string::npos);
        BOOST_ASSERT(response.find(TESTPATH1) == std::string::npos);
        BOOST_ASSERT(response.find(TESTPATH2) == std::string::npos);
    }

}
