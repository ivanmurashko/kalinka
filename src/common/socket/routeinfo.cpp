/**
   @file routeinfo.cpp
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

#include <boost/assert.hpp>

#include "routeinfo.h"

using namespace klk;
using namespace klk::sock;


// Constructor
RouteInfo::RouteInfo(const std::string& host, const u_int port,
                     const sock::Protocol proto, const sock::Type type) :
    m_host(host), m_port(port), m_proto(proto), m_type(type)
{
    if (m_proto == sock::TCPIP)
    {
        // only unicast is allowed
        BOOST_ASSERT(m_type == sock::UNICAST);
    }
}

// Copy constructor
RouteInfo::RouteInfo(const RouteInfo& value) :
    m_host(value.m_host), m_port(value.m_port),
    m_proto(value.m_proto), m_type(value.m_type)
{
}


// Destructor
RouteInfo::~RouteInfo()
{
}

// Compare operator
bool RouteInfo::operator==(const RouteInfo& value) const throw()
{
    return (m_host == value.m_host && m_port == value.m_port &&
            m_proto == value.m_proto);
}

// Compare operator
bool RouteInfo::operator!=(const RouteInfo& value) const throw()
{
    return (m_host != value.m_host || m_port != value.m_port ||
            m_proto != value.m_proto);
}


// Assigment operator
RouteInfo& RouteInfo::operator=(const RouteInfo& value)
{
    m_host = value.m_host;
    m_port = value.m_port;
    m_proto = value.m_proto;
    m_type = value.m_type;

    return *this;
}

