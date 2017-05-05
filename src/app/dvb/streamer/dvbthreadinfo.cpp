/**
   @file dvbthreadinfo.cpp
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
   - 2009/06/07 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "dvbthreadinfo.h"

using namespace klk;
using namespace klk::dvb::stream;

//
// ThreadInfo class
//

// Constructor
ThreadInfo::ThreadInfo(const IThreadPtr& thread, const IDevPtr& dev) :
    m_thread(thread), m_dev(dev), m_stream()
{
    BOOST_ASSERT(m_thread);
    BOOST_ASSERT(m_dev);
}

// Constructor
ThreadInfo::ThreadInfo(const IThreadPtr& thread, const IDevPtr& dev,
                       const IStreamPtr& stream) :
    m_thread(thread), m_dev(dev), m_stream(stream)
{
    BOOST_ASSERT(m_thread);
    BOOST_ASSERT(m_dev);
    BOOST_ASSERT(m_stream);
}

ThreadInfo::~ThreadInfo()
{
}

// Checks is the thread correspond to the specified
// device or not
const bool ThreadInfo::matchDev(const IDevPtr& dev) const throw()
{
    return (m_dev == dev);
}

// Retrives IThread pointer
const IThreadPtr ThreadInfo::getThread() const throw()
{
    return m_thread;
}

// Retrives IStream pointer
const IStreamPtr ThreadInfo::getStream() const
{
    BOOST_ASSERT(m_stream);
    return m_stream;
}
