/**
   @file domain.cpp
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
   - 2009/05/28 created by ipp (Ivan Murashko)
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

#include "domain.h"
#include "exception.h"
#include "socket/exception.h"
#include "utils.h"

using namespace klk;
using namespace klk::sock;

//
// DomainSocket class
//

// Constructor
DomainSocket::DomainSocket() : TCPSocket()
{
}

// Constructor
DomainSocket::DomainSocket(int fd) : TCPSocket(fd)
{
    BOOST_ASSERT(m_sock.getDescriptor() >= 0);
}

// Destructor
DomainSocket::~DomainSocket()
{
}

// Do the connection
void DomainSocket::connect(const RouteInfo& route)
{
    // disconnect before
    //disconnect();
    BOOST_ASSERT(m_sock.getDescriptor() == -1);
    BOOST_ASSERT(route.getPort() == 0);
    BOOST_ASSERT(route.getHost().empty() == false);
    BOOST_ASSERT(route.getProtocol() == sock::UNIXDOMAIN);

    int fd = 0;
    if ((fd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in socket(): %s. File: %s",
                             errno, strerror(errno),
                             route.getHost().c_str());
    }
    BOOST_ASSERT(fd >= 0);

    struct sockaddr_un addr;
    size_t size = 0;
    gethostbyname(route, addr, size);

    if (::connect(fd,
                  reinterpret_cast<struct sockaddr*>(&addr),
                  size) != 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in connect(): %s. File: %s",
                             errno, strerror(errno),
                             route.getHost().c_str());
    }

    m_sock.setDescriptor(fd);
}

// Retrives peername
const std::string DomainSocket::getPeerName() const throw()
    try
    {
        BOOST_ASSERT(m_sock.getDescriptor() >= 0);

        struct sockaddr_un addr_un;
        struct sockaddr *sa = reinterpret_cast<struct sockaddr*>(&addr_un);
        socklen_t len = sizeof(struct sockaddr_un);
        if (getpeername(m_sock.getDescriptor(), sa, &len) < 0)
        {

            throw Exception(__FILE__, __LINE__,
                            "Error %d in getpeername(): %s",
                            errno, strerror(errno));
        }

        return addr_un.sun_path;
    }
    catch(...)
    {
        return "unknown_domain_socket_peername";
    }

// Helper function for host by name
void DomainSocket::gethostbyname(const RouteInfo& route,
                                 struct sockaddr_un& addr,
                                 size_t& size)
{
    BOOST_ASSERT(route.getPort() == 0);
    BOOST_ASSERT(route.getHost().empty() == false);
    BOOST_ASSERT(route.getProtocol() == sock::UNIXDOMAIN);
#ifdef DARWIN
    addr.sun_len = sizeof(addr);
#endif
    addr.sun_family = PF_LOCAL;
    strncpy(addr.sun_path, route.getHost().c_str(), sizeof(addr.sun_path) - 1);

    // The size of the address is
    // the offset of the start of the filename,
    // plus its length,
    // plus one for the terminating null byte
    size = (offsetof(struct sockaddr_un, sun_path)
                   + strlen (addr.sun_path) + 1);
}

// Checks connection
Result DomainSocket::checkConnection(const RouteInfo& route)
{
    BOOST_ASSERT(route.getHost().empty() == false);
    BOOST_ASSERT(route.getPort() == 0);
    BOOST_ASSERT(route.getProtocol() == sock::UNIXDOMAIN);

    struct sockaddr_un addr;
    size_t size = 0;
    DomainSocket::gethostbyname(route, addr, size);

    int fd = 0;
    if ((fd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in socket(): %s",
                             errno, strerror(errno));
    }
    BOOST_ASSERT(fd >= 0);

    if (::connect(fd, reinterpret_cast<struct sockaddr*>(&addr), size) != 0)
    {
        close(fd);
        return ERROR;
    }

    close(fd);
    return OK;

}


//
// DomainListener class
//

// Constructor
DomainListener::DomainListener(const RouteInfo& route) : Listener(route)
{
    try
    {
        init();
    }
    catch(...)
    {
        disconnect();
        throw;
    }
}

// Destructor
DomainListener::~DomainListener()
{
}

// Stops accept in the listener
void DomainListener::stop() throw()
{
    Listener::stop();
    try
    {
        base::Utils::unlink(m_route.getHost());
    }
    catch(...)
    {
        // ignore unlink errors
    }
}


// Gets sockets
const ISocketPtr DomainListener::accept()
{
    // FIXME!!! infinity
    if (m_sock.checkData(-1) == ERROR)
    {
        // stopped
        throw Exception(__FILE__, __LINE__,
                             "Domain listener on '%s' has been stopped",
                             m_route.getHost().c_str());
    }
    // we have an request
    int fd = 0;
    struct sockaddr sa;
    bzero(&sa, sizeof(sa));
    socklen_t len = sizeof(sa);
    if ((fd = ::accept(m_sock.getDescriptor(), &sa, &len)) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in accept(): %s",
                             errno, strerror(errno));
    }

    return ISocketPtr(new DomainSocket(fd));
}

// Inits the listener
void DomainListener::init()
{
    BOOST_ASSERT(m_sock.getDescriptor() == -1); // only one time

    // remove prev sock
    base::Utils::unlink(m_route.getHost());

    int sock = -1;
    if ((sock = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0)
    {
        int saved_errno = errno;
        throw Exception(__FILE__, __LINE__,
                             "Error %d in socket(): %s",
                             saved_errno, strerror(saved_errno));
    }

    BOOST_ASSERT(sock >= 0);

    u_int yes = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
                   &yes, sizeof(u_int)) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in setsockopt(): %s",
                             errno, strerror(errno));
    }

#ifdef DARWIN
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT,
                   &yes, sizeof(u_int)) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in setsockopt(): %s",
                             errno, strerror(errno));
    }
#endif

    m_sock.setDescriptor(sock);

    struct sockaddr_un addr;
    size_t size = 0;
    DomainSocket::gethostbyname(m_route, addr, size);

    if (bind(m_sock.getDescriptor(),
             reinterpret_cast<struct sockaddr*>(&addr), size) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in bind(): %s. Path: %s.",
                             errno, strerror(errno),
                             m_route.getHost().c_str());
    }

    if (listen(m_sock.getDescriptor(), SOMAXCONN) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in listen(): %s",
                             errno, strerror(errno));
    }
}
