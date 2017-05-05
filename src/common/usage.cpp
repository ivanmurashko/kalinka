/**
   @file usage.cpp
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
   - 2009/05/20 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <errno.h>

#include "usage.h"
#include "exception.h"

using namespace klk;

//
// Usage class
//

// Default constructor
Usage::Usage() :
    Mutex(), m_who(), m_start_time(), m_last_usage()
{
    init();
}

// Constructor
Usage::Usage(int who) :
    Mutex(), m_who(who), m_start_time(), m_last_usage()
{
    init();
}

// Destructor
Usage::~Usage()
{
}

// Retrive CPU usage info in the following interval
// (0, 1)
const double Usage::getCPUUsage()
{
    struct timeval current;
    if (gettimeofday(&current, NULL) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Error %d in gettimeofday(): %s",
                        errno, strerror(errno));
    }
    struct rusage usg;
    if (getrusage(m_who, &usg) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Error %d in getrusage(): %s",
                        errno, strerror(errno));
    }

    Locker lock(this);

    // calculate cpu
    struct timeval initial_cpu;
    timeradd(&m_last_usage.ru_utime, &m_last_usage.ru_stime, &initial_cpu);
    struct timeval final_cpu;
    timeradd(&usg.ru_utime, &usg.ru_stime, &final_cpu);
    struct timeval cpu;
    timersub(&final_cpu, &initial_cpu, &cpu);

    // calculate total
    struct timeval total;
    timersub(&current, &m_start_time, &total);

    // calculate result
    double res = timeval2Double(cpu)/timeval2Double(total);

    // Save current measurments for next run.
    // getrusage() is not very precise (typically .01 sec).
    // Short time periods generate innacurate information.
    // (taken from :
       /*
       ** SellaNMS
       ** $Id: trace.cxx,v 1.8 2006/01/25 06:21:34 sella Exp $
       ** Copyright (c) 2001-2005 Digital Genesis Software, LLC.
       ** All Rights Reserved.
       ** Released under the GPL Version 2 License.
       ** http://www.digitalgenesis.com
       */
    // )
    if (total.tv_sec > 30) // don't update very often
    {
        memcpy(&m_last_usage, &usg, sizeof(struct rusage));
        memcpy(&m_start_time, &current, sizeof(struct timeval));
    }

    return res;
}

// Helper function to convert timeval to double
// @param[in] tv - the value to be converted
const double Usage::timeval2Double(const struct timeval& tv)
{
    return (tv.tv_sec + 1.0e-6*tv.tv_usec);
}

// Inits usage to use
void Usage::init()
{
    if (gettimeofday(&m_start_time, NULL) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Error %d in gettimeofday(): %s",
                        errno, strerror(errno));
    }
    if (getrusage(m_who, &m_last_usage) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Error %d in getrusage(): %s",
                        errno, strerror(errno));
    }
}

