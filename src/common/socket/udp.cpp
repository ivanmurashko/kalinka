/**
   @file udp.cpp
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
#include <arpa/inet.h>
#include <netinet/in.h>

#include "udp.h"
#include "exception.h"
#include "socket/exception.h"

using namespace klk;
using namespace klk::sock;

//
// UDPSocket class
//

// Constructor
UDPSocket::UDPSocket() : Socket()
{
}

// Constructor
UDPSocket::UDPSocket(int fd) : Socket(fd)
{
}

// Sends data to the specified connection
void UDPSocket::send(const RouteInfo& route, const BinaryData& data)
{
    // descriptor should not be set
    BOOST_ASSERT(m_sock.getDescriptor() < 0);

    if (data.empty())
        return; // no data nothing to send

    // get addr
    struct sockaddr_in addr;
    gethostbyname(route, addr);

    // send it
    int fd = -1;
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                                 "Error %d in socket(): %s",
                             errno, strerror(errno));
    }

    // wrapper
    Raw sock(fd);

    // do the send
    int err = ::sendto(sock.getDescriptor(), data.toVoid(), data.size(), 0,
                       reinterpret_cast<const struct sockaddr*>(&addr),
                       sizeof(addr));
    if (static_cast<size_t>(err) != data.size())
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in sendto(): %s",
                             errno, strerror(errno));
    }

    m_rater.updateOutput(data.size());
}

//
// UDPConnectSocket class
//

// Constructor
UDPConnectSocket::UDPConnectSocket() : UDPSocket()
{
}

// Destructor
UDPConnectSocket::~UDPConnectSocket()
{
}

// Recieves a data portion
void UDPConnectSocket::recv(BinaryData& data)
{
    BOOST_ASSERT(m_sock.getDescriptor() >= 0);
    BOOST_ASSERT(data.empty() == false);

    try
    {
        // check data within a specified period
        const time_t CHECK_INTERVAL = (10);
        if (m_sock.checkData(CHECK_INTERVAL) == ERROR)
        {
            // stopped
            throw ClosedConnection(__FILE__, __LINE__,
                                        "n/a");
        }

        int count =
            ::recv(m_sock.getDescriptor(), data.toVoid(), data.size(), 0);
        if (count < 0)
        {
            throw Exception(__FILE__, __LINE__,
                                 "Error %d in recv(): %s",
                                 errno,
                                 strerror(errno));
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
void UDPConnectSocket::connect(const RouteInfo& route)
{
    BOOST_ASSERT(m_sock.getDescriptor() == -1);
    BOOST_ASSERT(route.getPort() > 0);
    BOOST_ASSERT(route.getHost().empty() == false);
    // dont connect to multicast
    BOOST_ASSERT(route.getType() == sock::UNICAST);
    BOOST_ASSERT(route.getProtocol() == sock::UDP);

    int fd = 0;
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                                 "Error %d in socket(): %s",
                             errno, strerror(errno));
    }
    BOOST_ASSERT(fd >= 0);

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

//
// UDPListenSocket class
//

// Constructor
UDPListenSocket::UDPListenSocket(int fd) :
    UDPSocket(fd), m_event(), m_addr_len(0)
{
    BOOST_ASSERT(m_sock.getDescriptor() >= 0);
}

// Destructor
UDPListenSocket::~UDPListenSocket()
{
    Socket::disconnect();
}

// Disconnect the socket
void UDPListenSocket::disconnect() throw()
{
    m_event.stop();
}

// Sends a data portion
void UDPListenSocket::send(const BinaryData& data)
{
    BOOST_ASSERT(m_sock.getDescriptor() >= 0);
    BOOST_ASSERT(m_addr_len > 0);  /// should be set

    if (data.empty())
        return; // no data nothing to send

    // FIXME!!! code duplicate with Socket:send()
    // difference is only sendto vs send usage
    // the separate code here is ncessary because UDP accept result socket
    // does not know about third party address who initiated
    // the connection. Thus we should setup the connection info manually

    size_t chunk_size = data.size();
    // Mac OS X could not recive UDP packet greater tan SO_SNDBUF bytes long
    socklen_t optlen = sizeof(int);
    if (getsockopt(m_sock.getDescriptor(), SOL_SOCKET, SO_SNDBUF,
                   static_cast<size_t*>(&chunk_size), &optlen) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Error %d in getsockopt(): %s",
                        errno, strerror(errno));
    }

    // split data
    const u_char* begin = static_cast<const u_char*>(data.toVoid());
    const u_char* end = begin + data.size();
    for (const u_char* chunk = begin; chunk < end; chunk += chunk_size)
    {
        size_t size = std::min(static_cast<size_t>(end - chunk), chunk_size);
        int err = ::sendto(m_sock.getDescriptor(), chunk, size, 0,
                           reinterpret_cast<const struct sockaddr*>(&m_addr),
                           m_addr_len);
        if (static_cast<size_t>(err) != size)
        {
            throw Exception(__FILE__, __LINE__,
                             "Error %d in send(): %s",
                             errno, strerror(errno));
        }
        m_rater.updateOutput(size);
    }
}

// Recieves a data portion
void UDPListenSocket::recv(BinaryData& data)
{
    BOOST_ASSERT(m_sock.getDescriptor() >= 0);
    BOOST_ASSERT(data.empty() == false);

    try
    {
        // check data within a specified period
        const time_t CHECK_INTERVAL = (10);
        if (m_sock.checkData(CHECK_INTERVAL) == ERROR)
        {
            // stopped
            throw ClosedConnection(__FILE__, __LINE__,
                                        "on the localhost");
        }

        m_addr_len = sizeof(m_addr);
        int count =
            ::recvfrom(m_sock.getDescriptor(), data.toVoid(), data.size(),
                       0,
                       reinterpret_cast<struct sockaddr*>(&m_addr),
                       &m_addr_len);
        if (count < 0)
        {
            throw Exception(__FILE__, __LINE__,
                                 "Error %d in recvfrom(): %s",
                                 errno,
                                 strerror(errno));
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

// Stops the UDP socket
void UDPListenSocket::stop() throw()
{
    m_event.stop();
    stopCheckData();
}

// Retrives peername
const std::string UDPListenSocket::getPeerName() const throw()
    try
    {
        if (m_addr_len == 0)
        {
            return "n/a"; // can not retrive peer name
        }

        // FIXME!!! add IPv6 support
        return Socket::addr2String(AF_INET, m_addr);
    }
    catch(...)
    {
        return "unknown_udp_listen_socket";
    }

// Do the connection
void UDPListenSocket::connect(const RouteInfo& route)
{
    // should never be called
    NOTIMPLEMENTED;
}

//
// UDPListener class
//

// Constructor
UDPListener::UDPListener(const RouteInfo& route) : Listener(route),
                                                   m_current(),
                                                   m_stop()
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
UDPListener::~UDPListener()
{
}

// Stops accept in the listener
void UDPListener::stop() throw()
{
    m_stop.stop();
    if (m_current)
    {
        m_current->stop();
    }
    unjoin();
    Listener::stop();
}

// Inits the listener
void UDPListener::init()
{
    BOOST_ASSERT(m_sock.getDescriptor() == -1); // only one time

    int sock = -1;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
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

    u_int buffsize = 240*1024;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF,
                   &buffsize, sizeof(u_int)) < 0)
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


    bind();

    if (m_route.getType() == sock::MULTICAST)
    {
        join();
    }
}

// Bind unicast
void UDPListener::bind()
{
    struct sockaddr_in addr;
    Socket::gethostbyname(m_route, addr);
    if (::bind(m_sock.getDescriptor(),
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
}

// Join multicast
void UDPListener::join()
{
    struct ip_mreq mreq;

    mreq.imr_multiaddr.s_addr = inet_addr(m_route.getHost().c_str());
    mreq.imr_interface.s_addr = INADDR_ANY;
    if (setsockopt(m_sock.getDescriptor(), IPPROTO_IP, IP_ADD_MEMBERSHIP,
                   &mreq, sizeof(mreq)) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in setsockopt(): %s",
                             errno, strerror(errno));
    }
}

// Unjoin multicast
void UDPListener::unjoin() throw()
{
    if (m_route.getType() == sock::MULTICAST)
    {
        struct ip_mreq mreq;

        mreq.imr_multiaddr.s_addr = inet_addr(m_route.getHost().c_str());
        mreq.imr_interface.s_addr = INADDR_ANY;
        if (setsockopt(m_sock.getDescriptor(), IPPROTO_IP, IP_DROP_MEMBERSHIP,
                       &mreq, sizeof(mreq)) < 0)
        {
            klk_log(KLKLOG_ERROR, "Error %d in setsockopt(): %s",
                    errno, strerror(errno));
        }
    }
}

// Gets sockets
// return the socket immedeatelly
const ISocketPtr UDPListener::accept()
{
    if (m_current)
    {
        m_current->wait();
    }

    if (!m_stop.isStopped())
    {
        if (m_current == NULL)
        {
            int sock = m_sock.getDescriptor();
            m_current = UDPListenSocketPtr(new UDPListenSocket(sock));
        }
        else
        {
            // init for new cicle
            m_current->init();
        }
    }
    else
    {
        m_current.reset();
    }

    return m_current;
}

