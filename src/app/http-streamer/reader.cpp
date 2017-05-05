/**
   @file reader.cpp
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
   - 2009/04/28 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "reader.h"
#include "exception.h"

using namespace klk;
using namespace klk::http;

//
// Reader class
//

// Constructor
Reader::Reader(const ISocketPtr& sock, const time_t wait_interval) :
    m_sock(sock), m_broken_count_lock(), m_broken_count(0),
    m_wait_interval(wait_interval)
{
    BOOST_ASSERT(m_sock);
}

// Destructor
Reader::~Reader()
{
}

void Reader::stop() throw()
{
    if (m_sock)
    {
        m_sock->stopCheckData();
    }
}

// Retrives peer name
const std::string Reader::getPeerName() const
{
    BOOST_ASSERT(m_sock);
    return m_sock->getPeerName();
}

// @see Socket::checkData
const Result Reader::checkData()
{
    BOOST_ASSERT(m_sock);
    return m_sock->checkData(m_wait_interval);
}

// Increases broken package count
void Reader::increaseBrokenCount()
{
    Locker lock(&m_broken_count_lock);
    m_broken_count++;
}

// Retrives broken packages count
const u_long Reader::getBrokenCount() const
{
    Locker lock(&m_broken_count_lock);
    return m_broken_count;
}

// Retrives rate
const double Reader::getRate() const
{
    BOOST_ASSERT(m_sock);
    return m_sock->getInputRate();
}
