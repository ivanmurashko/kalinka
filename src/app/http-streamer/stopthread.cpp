/**
   @file stopthread.cpp
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
   - 2009/03/10 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "stopthread.h"
#include "exception.h"
#include "httpfactory.h"

using namespace klk;
using namespace klk::http;

// Constructor
StopThread::StopThread(Factory* factory) :
    Thread(factory), m_data(), m_wait()
{
}

// Destructor
StopThread::~StopThread()
{
}

// main thread body
void StopThread::start()
{
    while (!isStopped())
    {
        m_wait.startWait(0);
        try
        {
            while (m_data.empty() == false && !isStopped())
            {
                ConnectThreadPtr data = m_data.retrive_front();
                getFactory()->getScheduler()->stopThread(data);
            }
        }
        catch(const std::exception& err)
        {
            klk_log(KLKLOG_ERROR, "Error detected during stop a "
                    "connection thread: %s",
                    err.what());
        }
    }
}

// Stops thread
void StopThread::stop() throw()
{
    base::Thread::stop();
    m_wait.stopWait();
}

// Adds a stop thread
void StopThread::addConnectThread(const ConnectThreadPtr& thread)
{
    m_data.push_back(thread);
    m_wait.stopWait();
}

