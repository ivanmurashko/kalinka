/**
   @file factory.cpp
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
   - 2009/03/07 created by ipp (Ivan Murashko)
   - 2009/05/17 socket.cpp -> ./socket/factory.cpp
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>

#include "socket.h"
#include "tcp.h"
#include "udp.h"
#include "domain.h"
#include "exception.h"

using namespace klk;
using namespace klk::sock;

//
// Factory class
//

const IListenerPtr Factory::getListener(const RouteInfo& route)
{
    IListenerPtr listener;
    try
    {
        switch (route.getProtocol())
        {
        case sock::UNIXDOMAIN:
            listener = IListenerPtr(new DomainListener(route));
            break;
        case sock::TCPIP:
            listener = IListenerPtr(new TCPListener(route));
            break;
        case sock::UDP:
            listener = IListenerPtr(new UDPListener(route));
            break;
        default:
            break;
        }
    }
    catch(const std::bad_alloc&)
    {
        throw Exception(__FILE__, __LINE__, err::MEMORYALLOC);
    }
    BOOST_ASSERT(listener);
    return listener;
}


// Creates socket specified for a protocol
const ISocketPtr Factory::getSocket(const sock::Protocol proto)
{
    ISocketPtr sock;
    try
    {
        switch (proto)
        {
        case sock::UNIXDOMAIN:
            sock = ISocketPtr(new DomainSocket());
            break;
        case sock::TCPIP:
            sock = ISocketPtr(new TCPSocket());
            break;
        case sock::UDP:
            sock = ISocketPtr(new UDPConnectSocket());
            break;
        default:
            break;
        }
    }
    catch(const std::bad_alloc&)
    {
        throw Exception(__FILE__, __LINE__, err::MEMORYALLOC);
    }
    BOOST_ASSERT(sock);
    return sock;
}


// Checks connection
Result Factory::checkConnection(const RouteInfo& route)
{
    Result rc = ERROR;

    switch (route.getProtocol())
    {
    case sock::TCPIP:
        rc = TCPSocket::checkConnection(route);
        break;
    case sock::UNIXDOMAIN:
        rc = DomainSocket::checkConnection(route);
        break;
    default:
        throw Exception(__FILE__, __LINE__,
                             "Unsupported socket protocol");
    }

    return rc;
}
