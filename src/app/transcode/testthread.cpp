/**
   @file testthread.cpp
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
   - 2009/04/02 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cppunit/extensions/HelperMacros.h>

#include "testdefines.h"
#include "testtransthread.h"
#include "utils.h"
#include "exception.h"
#include "socket/exception.h"
#include "testutils.h"

// module specific
#include "network/defines.h"

using namespace klk;
using namespace klk::trans;
using namespace klk::trans::test;

//
// TestThread class
//

// Constructor
TestThread::TestThread() :
    klk::test::Thread(), m_sock(), m_data()
{
}

// Starts the thread
void TestThread::mainLoop()
{
    try
    {
        sock::RouteInfo route(ROUTEADDR, ROUTEPORT,
                        sock::TCPIP, sock::UNICAST);

        klk_log(KLKLOG_DEBUG, "Transcode test: started thread. "
                "Host: %s. Port: %u",
                route.getHost().c_str(),
                route.getPort());

        // initialization
        m_listener = sock::Factory::getListener(route);
        while(!isStopped())
        {
            // FIXME!!! add lock here
            m_sock = m_listener->accept();
            processConnection();
        }
        m_sock.reset();
    }
    catch(const ClosedConnection&)
    {
        // ignore closed connections exceptions
        // it's ok - EOS was gotten
    }
    catch(...)
    {
        if (!isStopped())
        {
            Locker lock(&m_lock);
            m_listener.reset();
            m_sock.reset();
        }
        throw;
    }
}

// Processing a connection
void TestThread::processConnection()
{
    m_data = klk::BinaryData();
    try
    {
        while(!isStopped())
        {
            BOOST_ASSERT(m_sock);
            if (m_sock->checkData(20) == OK)
            {
                const size_t buffsize = 1024;
                // recieve data
                BinaryData buff(buffsize);
                m_sock->recv(buff);
                if (buff.empty() == false)
                {
                    m_data.add(buff);
                }
            }
        }
    }
    catch(const ClosedConnection&)
    {
        // ignore closed connections exceptions
        // it's ok - EOS was gotten
    }
    catch(...)
    {
        if (!isStopped())
        {
            Locker lock(&m_lock);
            m_listener.reset();
            m_sock.reset();
        }
        throw;
    }
    if (m_rc == OK)
    {
        // final step - saving result
        //BOOST_ASSERT(data.empty() == false);
        base::Utils::saveData2File(OUTPUTROUTE, m_data);
    }
}

// Stops the thread
void TestThread::stop() throw()
{
    klk::test::Thread::stop();
    Locker lock(&m_lock);
    if (m_listener)
        m_listener->stop();
    if (m_sock)
        m_sock->stopCheckData();
}
