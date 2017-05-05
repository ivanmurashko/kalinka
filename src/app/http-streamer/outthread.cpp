/**
   @file outthread.cpp
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
   - 2009/03/09 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "outthread.h"
#include "exception.h"
#include "conthread.h"
#include "httpfactory.h"

using namespace klk;
using namespace klk::http;

//
// OutThread class
//

// Constructor
OutThread::OutThread(Factory* factory) :
    RouteThread(factory)
{
}

// Destructor
OutThread::~OutThread()
{
}

// Main thread's body
void OutThread::start()
{
    while (!isStopped())
    {
        try
        {
            ISocketPtr sock = getListener()->accept();
            ConnectThreadPtr thread(new ConnectThread(getFactory(), sock));
            getFactory()->getConnectThreadContainer()->startConnectThread(
                thread);
        }
        catch(const std::exception& err)
        {
            klk_log(KLKLOG_ERROR, "Got an exception in HTTP out thread: %s",
                    err.what());
        }
        catch(...)
        {
            klk_log(KLKLOG_ERROR,
                    "Got an unknown exception in HTTP out thread");
        }
    }
    resetListener();
}

// Retrives rate
const double OutThread::getRate() const
{
    NOTIMPLEMENTED;
    return 0.0;
}

