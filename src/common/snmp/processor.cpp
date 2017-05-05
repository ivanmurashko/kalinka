/**
   @file processor.cpp
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
   - 2009/05/26 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "processor.h"
#include "exception.h"

using namespace klk;
using namespace snmp;

//
// Processor class
//

// Constructor
Processor::Processor(DataProcessor f, const std::string& sockname) :
    Thread(),m_sockname(sockname),  m_f(f), m_listener(),
    m_sock(), m_sock_lock()
{
}

// Destructor
Processor::~Processor()
{
}

// Does initialization
void Processor::init()
{
    // basic initialization
    Thread::init();
    // init listener
    m_listener =
        sock::Factory::getListener(Protocol::getRouteInfo(m_sockname));
}

// Stops the thread
void Processor::stop() throw()
{
    Thread::stop();
    if (m_listener)
    {
        m_listener->stop();
    }

    stopSock();
}

// Gets socket
const ISocketPtr Processor::getSock() const
{
    Locker lock(&m_sock_lock);
    return m_sock;
}

// Sets socket
void Processor::setSock(const ISocketPtr& sock)
{
    Locker lock(&m_sock_lock);
    m_sock = sock;
}

// Stops socket check data
void Processor::stopSock() throw()
{
    Locker lock(&m_sock_lock);
    if (m_sock)
    {
        m_sock->stopCheckData();
    }
}

// Resets socket check data
void Processor::resetSock() throw()
{
    Locker lock(&m_sock_lock);
    m_sock.reset();
}


// Starts the thread
void Processor::start()
{
    const time_t FINAL_CHECKDATA(5);
    // main loop
    while (!isStopped())
    {
        resetSock();
        try
        {
            setSock(m_listener->accept());
            Protocol proto(getSock());
            // request
            const IDataPtr req = proto.recvData();
            // retrive response
            const IDataPtr res = m_f(req);
            // send the response back
            proto.sendData(res);
            // check for closing connections
            getSock()->checkData(FINAL_CHECKDATA);
        }
        catch(const std::exception& err)
        {
            // just log the error
            klk_log(KLKLOG_ERROR, "Got an error in SNMP processor: %s",
                    err.what());
        }
        catch(...)
        {
            // just log the error
            klk_log(KLKLOG_ERROR, "Got an unknown error in SNMP processor");
        }
    }
}
