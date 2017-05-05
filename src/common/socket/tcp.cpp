/**
   @file tcp.cpp
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

#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#ifdef LINUX
#include <netinet/tcp.h>
#endif //LINUX

#include "tcp.h"
#include "exception.h"
#include "socket/exception.h"

using namespace klk;
using namespace klk::sock;

//
// TCPSocket class
//

// Constructor
TCPSocket::TCPSocket() : Socket()
{
}

// Constructor
TCPSocket::TCPSocket(int fd) : Socket(fd)
{
    BOOST_ASSERT(m_sock.getDescriptor() >= 0);
}

// Destructor
TCPSocket::~TCPSocket()
{
}

// Check that data can be send i. e. check that socket ready
// to write
// @note timeout for wait is 10 seconds
void TCPSocket::checkReady4Send()
{
    // check that data can be sent
    fd_set wset;
    FD_ZERO(&wset);
    FD_SET(m_sock.getDescriptor(),  &wset);

    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    int status = select(m_sock.getDescriptor() + 1, NULL, &wset, NULL, &tv);

    if (status == -1)
    {
        // Network error
        throw Exception(__FILE__, __LINE__,
                        "Error %d in select(): %s. "
                        "Socket error. Network error",
                        errno, strerror(errno));
    }
    else if (status == 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Timeout exceed. Data can not be sent now");
    }

    /* Check possibility for output device receive data*/
    if(!FD_ISSET(m_sock.getDescriptor(), &wset))
    {
        throw Exception(__FILE__, __LINE__,
                        "Data can not be sent now");
    }
}

// Recieves a data portion
void TCPSocket::recv(BinaryData& data)
{
    BOOST_ASSERT(m_sock.getDescriptor() >= 0);
    BOOST_ASSERT(data.empty() == false);

    try
    {
        int count =
            ::recv(m_sock.getDescriptor(), data.toVoid(), data.size(), 0);
        if (count < 0)
        {
            int saved_errno = errno;
            if (saved_errno == EPIPE)
            {
                throw ClosedConnection(__FILE__, __LINE__,
                                            getPeerName());
            }
            else
            {
                throw Exception(__FILE__, __LINE__,
                                     "Error %d in recv(): %s",
                                     saved_errno,
                                     strerror(saved_errno));
            }
        }
        else if (count == 0)
        {
            throw ClosedConnection(__FILE__, __LINE__,
                                        getPeerName());
        }
        else if (count < static_cast<int>(data.size()))
        {
            data.resize(count);
        }

        m_rater.updateInput(static_cast<size_t>(count));
    }
    catch(...)
    {
        disconnect();
        throw;
    }
}

// Do the connection
void TCPSocket::connect(const RouteInfo& route)
{
    // disconnect before
    //disconnect();
    BOOST_ASSERT(m_sock.getDescriptor() == -1);
    BOOST_ASSERT(route.getPort() > 0);
    BOOST_ASSERT(route.getHost().empty() == false);
    BOOST_ASSERT(route.getProtocol() == sock::TCPIP);

    int fd = 0;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in socket(): %s",
                             errno, strerror(errno));
    }
    BOOST_ASSERT(fd >= 0);

#if 0
    u_int mincount = 10;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVLOWAT,
                   &mincount, sizeof(u_int)) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in setsockopt(): %s",
                             errno, strerror(errno));
    }
#endif

    struct sockaddr_in addr;
    gethostbyname(route, addr);

    if (::connect(fd,
                  reinterpret_cast<struct sockaddr*>(&addr),
                  sizeof(addr)) != 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in connect(): %s",
                             errno, strerror(errno));
    }

    m_sock.setDescriptor(fd);
}

// Retrives peername
const std::string TCPSocket::getPeerName() const throw()
    try
    {
        BOOST_ASSERT(m_sock.getDescriptor() >= 0);
        struct sockaddr sa;
        bzero(&sa, sizeof(sa));
        socklen_t len = sizeof(sa);
        if (getpeername(m_sock.getDescriptor(), &sa, &len) < 0)
        {
            throw Exception(__FILE__, __LINE__,
                            "Error %d in getpeername(): %s",
                            errno, strerror(errno));
        }

        const struct sockaddr_in*
            sin = reinterpret_cast<const struct sockaddr_in*>(&sa);
        return Socket::addr2String(sa.sa_family, *sin);
    }
    catch(...)
    {
        return "unknown_tcp_peer_name";
    }

// Sends data to the specified connection
void TCPSocket::send(const RouteInfo& route, const BinaryData& data)
{
    // mainly valid for UDP
    NOTIMPLEMENTED;
}

// Checks connection
Result TCPSocket::checkConnection(const RouteInfo& route)
{
    BOOST_ASSERT(route.getHost().empty() == false);
    BOOST_ASSERT(route.getPort() > 0);
    BOOST_ASSERT(route.getProtocol() == sock::TCPIP);

    struct sockaddr_in addr;
    Socket::gethostbyname(route, addr);

    int fd = 0;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in socket(): %s",
                             errno, strerror(errno));
    }
    BOOST_ASSERT(fd >= 0);

    if (::connect(fd,
                  reinterpret_cast<struct sockaddr*>(&addr),
                  sizeof(addr)) != 0)
    {
        close(fd);
        return ERROR;
    }

    close(fd);
    return OK;

}

// @copydoc klk::sock::ISocket::setKeepAlive
void TCPSocket::setKeepAlive(time_t timeout)
{
    BOOST_ASSERT(m_sock.getDescriptor() >= 0);

    int value = (timeout > 0) ? 1:0;

    if (setsockopt(m_sock.getDescriptor(), SOL_SOCKET, SO_KEEPALIVE,
                   &value, sizeof(value)) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in setsockopt(): %s",
                             errno, strerror(errno));
    }

#ifdef LINUX
    if (value)
    {

        // The time (in seconds) the connection needs to remain
        // idle before TCP starts sending keepalive probes,
        //  if the socket option SO_KEEPALIVE has been set on this socket.
        value = timeout;

        if (setsockopt(m_sock.getDescriptor(), SOL_TCP, TCP_KEEPIDLE,
                       &value, sizeof(value)) < 0)
        {
            throw Exception(__FILE__, __LINE__,
                            "Error %d in setsockopt(): %s",
                            errno, strerror(errno));
        }


        //  The time (in seconds) between individual keepalive probes.
        value = timeout;

        if (setsockopt(m_sock.getDescriptor(), SOL_TCP, TCP_KEEPINTVL,
                       &value, sizeof(value)) < 0)
        {
            throw Exception(__FILE__, __LINE__,
                            "Error %d in setsockopt(): %s",
                            errno, strerror(errno));
        }


        //  The maximum number of keepalive probes TCP should send
        // before dropping the connection.
        value = 3;
        if (setsockopt(m_sock.getDescriptor(), SOL_TCP, TCP_KEEPCNT,
                       &value, sizeof(value)) < 0)
        {
            throw Exception(__FILE__, __LINE__,
                            "Error %d in setsockopt(): %s",
                            errno, strerror(errno));
        }

    }
#endif //LINUX
}


//
// TCPListener class
//

// Constructor
TCPListener::TCPListener(const RouteInfo& route) : Listener(route)
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
TCPListener::~TCPListener()
{

}

// Gets sockets
const ISocketPtr TCPListener::accept()
{
    // FIXME!!! infinity
    if (m_sock.checkData(-1) == ERROR)
    {
        // stopped
        throw Exception(__FILE__, __LINE__,
                             "TCP/IP listener on '%s:%d' has been stopped",
                             m_route.getHost().c_str(),
                             m_route.getPort());
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

    return ISocketPtr(new TCPSocket(fd));
}

// Inits the listener
void TCPListener::init()
{
    BOOST_ASSERT(m_sock.getDescriptor() == -1); // only one time

    int sock = -1;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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

#if 0
    u_int mincount = 10;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVLOWAT,
                   &mincount, sizeof(u_int)) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in setsockopt(): %s",
                             errno, strerror(errno));
    }
#endif

    m_sock.setDescriptor(sock);

    struct sockaddr_in addr;
    Socket::gethostbyname(m_route, addr);
    if (bind(m_sock.getDescriptor(),
             reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in bind(): %s. Host: %s. Port: %d",
                             errno, strerror(errno),
                             m_route.getHost().c_str(),
                             m_route.getPort());
    }

    // setup route info
    // retrive host and port that was set after bind
    updateRouteInfo();

    if (listen(m_sock.getDescriptor(), SOMAXCONN) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in listen(): %s",
                             errno, strerror(errno));
    }

}


#if 0

//
// Listener class
//

// Constructor
Listener::Listener() :
    m_route(NULL), m_sock(-1)
{
}

// Destructor
Listener::~Listener()
{
    clear();
}

// Inits listener
void Listener::init(const RouteInfo& route)
{
    BOOST_ASSERT(m_sock, -1); // only one time
    BOOST_ASSERT(m_route, NULL); // only one time

    if ((m_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        int saved_errno = errno;
        m_sock = -1;
        throw Exception(__FILE__, __LINE__,
                             "Error %d in socket(): %s",
                             saved_errno, strerror(saved_errno));
    }

    BOOST_ASSERT(m_sock >= 0);

    u_int yes = 1;
    if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR,
                   &yes, sizeof(u_int)) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in setsockopt(): %s",
                             errno, strerror(errno));
    }

#if 0
    u_int mincount = 10;
    if (setsockopt(m_sock, SOL_SOCKET, SO_RCVLOWAT,
                   &mincount, sizeof(u_int)) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in setsockopt(): %s",
                             errno, strerror(errno));
    }
#endif

    struct sockaddr_in addr;
    gethostbyname(route, addr);
    if (bind(m_sock,
             reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in bind(): %s. Host: %s. Port: %d",
                             errno, strerror(errno),
                             route.getHost().c_str(),
                             route.getPort());
    }

    // setup route info
    setRouteInfo();

    if (listen(m_sock, SOMAXCONN) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in listen(): %s",
                             errno, strerror(errno));
    }
}

// Stops listener processing
void Listener::stop() throw()
{
    try
    {
        if (m_route)
        {
            // init connection to break accept
            ConnectSocket::checkConnection(*m_route);
        }
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Got an exception in Listener::stop()");
    }
}

// Gets sockets
const SocketPtr Listener::accept()
{
    BOOST_ASSERT(m_sock >= 0);
    int fd = 0;
    struct sockaddr sa;
    bzero(&sa, sizeof(sa));
    socklen_t len = sizeof(sa);
    if ((fd = ::accept(m_sock, &sa, &len)) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in accept(): %s",
                             errno, strerror(errno));
    }

    return SocketPtr(new Socket(fd));
}

// Clears internal info
void Listener::clear() throw()
{
    if (m_sock >= 0)
    {
        close(m_sock);
        m_sock = -1;
    }
    KLKDELETE(m_route);
}

// Retrive route info
const RouteInfo Listener::getRouteInfo() const
{
    BOOST_ASSERT(m_route);
    return *m_route;
}

// Setups route info
void Listener::setRouteInfo()
{
    try
    {
        BOOST_ASSERT(m_route, NULL);
        struct sockaddr sa;
        bzero(&sa, sizeof(sa));
        socklen_t len = sizeof(sa);
        if (getsockname(m_sock, &sa, &len) < 0)
        {
            throw Exception(__FILE__, __LINE__,
                                 "Error %d in getsockname(): %s",
                                 errno, strerror(errno));
        }

        const struct sockaddr_in*
            sin = reinterpret_cast<const struct sockaddr_in*>(&sa);
        char addrstr[64];
        const void *addrp = static_cast<const void*>(&sin->sin_addr);
        inet_ntop(sa.sa_family, addrp, addrstr, sizeof(addrstr));

        int port = ntohs(sin->sin_port);
        m_route = new RouteInfo(addrstr, port,
                                net::TCPIP, net::UNICAST);
    }
    catch(const std::bad_alloc&)
    {
        close(m_sock);
        m_sock = -1;
        throw Exception(__FILE__, __LINE__, err::MEMORYALLOC);
    }
    catch(...)
    {
        close(m_sock);
        m_sock = -1;
        throw;
    }
}

#endif
