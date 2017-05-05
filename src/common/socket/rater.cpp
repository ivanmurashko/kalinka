/**
   @file rater.cpp
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
   - 2009/05/10 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rater.h"

using namespace klk;

//
// Rater class
//

// update interval
static const time_t UPDATE_INTERVAL = 22;

// Constructor
Rater::Rater() :
    m_lock(),
    m_in_rate(0), m_out_rate(0),
    m_in_size(0), m_out_size(0),
    m_in_last(time(NULL)), m_out_last(time(NULL))
{
}

// Destructor
Rater::~Rater()
{
}

// Updates received data size
void Rater::updateInput(const size_t size)
{
    time_t now = time(NULL);
    Locker lock(&m_lock);
    if (now - m_in_last > UPDATE_INTERVAL)
    {
        m_in_rate = static_cast<double>(m_in_size)/(now - m_in_last);
        m_in_last = now;
        m_in_size = size;
    }
    else
    {
        m_in_size += size;
    }
}

// Updates sent data size
void Rater::updateOutput(const size_t size)
{
    time_t now = time(NULL);
    Locker lock(&m_lock);
    if (now - m_out_last > UPDATE_INTERVAL)
    {
        m_out_rate = static_cast<double>(m_out_size)/(now - m_out_last);
        m_out_last = now;
        m_out_size = size;
    }
    else
    {
        m_out_size += size;
    }
}

// Retrives input rate
const double Rater::getInputRate() const
{
    Locker lock(&m_lock);
    if (m_in_rate < 0.001)
    {
        time_t now = time(NULL);
        if (now > m_in_last)
            return static_cast<double>(m_in_size)/(now - m_in_last);
    }
    return m_in_rate;
}

// Retrives output rate
const double Rater::getOutputRate() const
{
    Locker lock(&m_lock);
    if (m_out_rate < 0.001)
    {
        time_t now = time(NULL);
        if (now > m_out_last)
            return static_cast<double>(m_out_size)/(now - m_out_last);
    }
    return m_out_rate;
}

