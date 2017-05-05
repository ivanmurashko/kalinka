/**
   @file base.cpp
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

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "base.h"
#include "exception.h"
#include "utils.h"

using namespace klk;
using namespace klk::sock;

//
// Raw class
//

/**
   Stop data
*/
static const std::string STOPDATA("stopdata");

// Constructor
Raw::Raw() : m_sock(-1), m_lock()
{
    if (pipe(m_stop) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Error %d in pipe(): %s",
                        errno, strerror(errno));
    }
}

// Constructor
Raw::Raw(int sock) : m_sock(sock), m_lock()
{
    BOOST_ASSERT(m_sock >= 0);
    if (pipe(m_stop) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Error %d in pipe(): %s",
                        errno, strerror(errno));
    }
}

// Destructor
Raw::~Raw()
{
    disconnect();

    close(m_stop[0]);
    close(m_stop[1]);
}

// Checks is there any data available at the socket or not
Result Raw::checkData(time_t timeout)
{
    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(m_stop[0], &rset);
    FD_SET(m_sock, &rset);

    int maxfd = std::max(m_sock, m_stop[0]) + 1;

    int status = 0;

    if (timeout < 0)
    {
        status = select(maxfd, &rset, NULL, NULL, NULL);
    }
    else
    {
        struct timeval tv;
        tv.tv_sec = timeout;
        if (!timeout)
        {
            // not nil value
            tv.tv_usec = 1;
        }
        else
        {
            tv.tv_usec = 0;
        }

        status = select(maxfd, &rset, NULL, NULL, &tv);
    }


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
        // Timeout exceed
        klk_log(KLKLOG_DEBUG,
                "Socket timeout exceeded. No data");
        return ERROR;
    }

    if (FD_ISSET(m_stop[0], &rset))
    {
        klk_log(KLKLOG_DEBUG,
                "Socket::checkData() was interapted");
        // read the stop data
        // to have a possibility to call
        // stopCheckData several times
        std::vector<char> buff(STOPDATA.size());
        if (read(m_stop[0], &buff[0], buff.size()) < 0)
        {
            throw Exception(__FILE__, __LINE__,
                            "Error %d in read(): %s",
                            errno, strerror(errno));
        }
        return ERROR;
    }

    if(FD_ISSET(m_sock, &rset) /* input device is ready for reading*/ )
    {
        return OK;
    }
    return ERROR;
}

// Stops data check
void Raw::stopCheckData() throw()
{
    int err = ::write(m_stop[1], STOPDATA.c_str(), STOPDATA.size());
    if (err != static_cast<int>(STOPDATA.size()))
    {
        klk_log(KLKLOG_ERROR,
                "Socket::stopCheckData() failed. "
                "Error %d in write(): %s",
                errno, strerror(errno));
    }
}

// Closes connection
void Raw::disconnect() throw()
{
    Locker lock(&m_lock);
    if (m_sock != -1)
    {
        klk_close(m_sock);
        m_sock = -1;
    }
}

// Retrives descriptor
const int Raw::getDescriptor() const throw()
{
    Locker lock(&m_lock);
    return m_sock;
}

// Sets descriptor
void Raw::setDescriptor(int sock)
{
    Locker lock(&m_lock);
    disconnect();
    m_sock = sock;
}

//
// Socket class
//

// Default constructor
Socket::Socket() : m_sock(), m_rater()
{
}

// Constructor from fd
Socket::Socket(int fd) : m_sock(fd), m_rater()
{
}

// Destructor
Socket::~Socket()
{
    m_sock.disconnect();
}

// Recieves all required data portion
void Socket::recvAll(BinaryData& data)
{
    size_t required_size = data.size();
    recv(data);
    while(data.size() != required_size)
    {
        BOOST_ASSERT(data.size() < required_size);
        if (checkData(1) == OK)
        {
            BinaryData tmp(required_size - data.size());
            recv(tmp);
            // should not be empty
            BOOST_ASSERT(tmp.empty() == false);

            data.add(tmp);
        }
    }
}


// Closes connection
void Socket::disconnect() throw()
{
    m_sock.stopCheckData();
    m_sock.disconnect();
}

// Helper function for host by name
void Socket::gethostbyname(const RouteInfo& route,
                           struct sockaddr_in& addr)
{
    bzero(&addr, sizeof(addr));

    if (route.getHost().empty() == false)
    {
#if HAVE_GETHOSTBYNAME_R == 1
        struct hostent he, *phe = NULL;
        BinaryData buff(2048);
        int fnord = 0;
        if (gethostbyname_r(route.getHost().c_str(), &he,
                            static_cast<char*>(buff.toVoid()),
                            buff.size(), &phe, &fnord))
        {
            throw Exception(__FILE__, __LINE__,
                            "Error %d in gethostbyname_r(): %s",
                            fnord, strerror(fnord));
        }
#elif HAVE_GETHOSTBYNAME_R == 0
        struct hostent *phe = ::gethostbyname(route.getHost().c_str());
#else
#error "No have gethostbyname_r check"
#endif
        BOOST_ASSERT(phe);
        memmove(&addr.sin_addr.s_addr, phe->h_addr, sizeof(in_addr));
    }
    else
    {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(route.getPort());
}

// Retrive input rate
const double Socket::getInputRate() const
{
    return m_rater.getInputRate();
}

// Retrive output rate
const double Socket::getOutputRate() const
{
    return m_rater.getOutputRate();
}

// Checks is there any data available at the socket or not
Result Socket::checkData(time_t timeout)
{
    return m_sock.checkData(timeout);
}

// Stops data check
void Socket::stopCheckData() throw()
{
    m_sock.stopCheckData();
}

// Converts struct sockaddr_in into human readable
// form
const std::string Socket::addr2String(int af, const struct sockaddr_in& addr)
{
    char addrstr[64];
    const void *addrp = static_cast<const void*>(&addr.sin_addr);
    inet_ntop(af, addrp, addrstr, sizeof(addrstr));
    return addrstr;
}

// Sends a data portion
void Socket::send(const BinaryData& data)
{
    BOOST_ASSERT(m_sock.getDescriptor() >= 0);

    if (data.empty())
        return; // no data nothing to send

    size_t chunk_size = data.size();

    // During test on Mac OS X I discovered that it could not recive
    // UDP packet greater tan SO_SNDBUF bytes long
    // thus we split all data into packets that are no more than
    // the buffer length
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
        int err = ::send(m_sock.getDescriptor(), chunk, size, 0);
        if (static_cast<size_t>(err) != size)
        {
            throw Exception(__FILE__, __LINE__,
                             "Error %d in send(): %s",
                             errno, strerror(errno));
        }
        m_rater.updateOutput(size);
    }
}

// @copydoc klk::sock::ISocket::setSendTimeout
void Socket::setSendTimeout(time_t timeout)
{
    BOOST_ASSERT(m_sock.getDescriptor() >= 0);
    struct timeval value;
    value.tv_sec = timeout;
    value.tv_usec = 0;
    if (setsockopt(m_sock.getDescriptor(), SOL_SOCKET, SO_SNDTIMEO,
                   &value, sizeof(value)) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                             "Error %d in setsockopt(): %s",
                             errno, strerror(errno));
    }
}

// @copydoc klk::sock::ISocket::setKeepAlive
void Socket::setKeepAlive(time_t timeout)
{
    klk_log(KLKLOG_DEBUG, "setKeepAlive() is not supported and ignored");
}

//
// Listener class
//

// Constructor
Listener::Listener(const RouteInfo& route) : m_route(route), m_sock()
{
}

// Destructor
Listener::~Listener()
{
    disconnect();
}

// Disconnects the socket
void Listener::disconnect() throw()
{
    m_sock.disconnect();
}


// Updates socket info after bind
void Listener::updateRouteInfo()
{
    BOOST_ASSERT(m_sock.getDescriptor() >= 0);
    struct sockaddr sa;
    bzero(&sa, sizeof(sa));
    socklen_t len = sizeof(sa);
    if (getsockname(m_sock.getDescriptor(), &sa, &len) < 0)
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

    m_route = RouteInfo(addrstr, port,
                        m_route.getProtocol(), m_route.getType());
}
