/**
   @file protocol.cpp
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
   - 2009/05/26 created by ipp (Ivan Murashko)
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
#include <sys/socket.h>
#include <sys/un.h>

#include "protocol.h"
#include "exception.h"
#include "scalar.h"
#include "table.h"

using namespace klk;
using namespace klk::snmp;

//
// Protocol class
//

// Constructor
Protocol::Protocol(const std::string& sockname) :
    m_sock(sock::Factory::getSocket(sock::UNIXDOMAIN))
{
    BOOST_ASSERT(m_sock);

    // do connection
    m_sock->connect(Protocol::getRouteInfo(sockname));
}

// Constructor
Protocol::Protocol(const ISocketPtr& sock) :
    m_sock(sock)
{
    BOOST_ASSERT(m_sock);
}

// Destructor
Protocol::~Protocol()
{
    m_sock.reset();
}

// Retrives real path from socket name
const sock::RouteInfo Protocol::getRouteInfo(const std::string& sockname)
{
    if (*(sockname.begin()) != '/')
    {
        return sock::RouteInfo("/tmp/" + sockname, 0,
                              sock::UNIXDOMAIN, sock::UNICAST);
    }

    return sock::RouteInfo(sockname, 0,
                           sock::UNIXDOMAIN, sock::UNICAST);
}

/**
   Data header
*/
struct ProtocolHeader
{
    ValueType m_type; ///< value type
    size_t m_size; ///< data size
};

// Sends data
void Protocol::sendData(const IDataPtr& data)
{
    BOOST_ASSERT(data);

    // data to be sent
    BinaryData serialize_data = data->serialize();

    // create header first
    BinaryData header(sizeof(ProtocolHeader));
    struct ProtocolHeader *header_raw =
        static_cast<struct ProtocolHeader*>(header.toVoid());

    header_raw->m_type = data->getType();
    header_raw->m_size = serialize_data.size();

    // send it
    m_sock->send(header);
    m_sock->send(serialize_data);
}

/**
   Wait interval
*/
const time_t WAIT_INTERVAL(4);

// Receives data
const IDataPtr Protocol::recvData()
{
    IDataPtr res;
    try
    {
        // retrive header
        if (m_sock->checkData(WAIT_INTERVAL) != OK)
        {
            throw Exception(__FILE__, __LINE__,
                                 "No data for SNMP protocol in %d seconds",
                                 WAIT_INTERVAL);
        }
        BinaryData header(sizeof(ProtocolHeader));
        m_sock->recvAll(header);
        BOOST_ASSERT(header.size() == sizeof(ProtocolHeader));
        const struct ProtocolHeader *header_raw =
            static_cast<struct ProtocolHeader*>(header.toVoid());
        BOOST_ASSERT(header_raw->m_size > 0);
        BinaryData data(header_raw->m_size);
        m_sock->recvAll(data);
        BOOST_ASSERT(data.size() == header_raw->m_size);

        switch(header_raw->m_type)
        {
        case SCALAR:
            res = IDataPtr(new Scalar(data));
            break;
        case TABLE:
            res = IDataPtr(new Table(data));
            break;
        default:
            throw Exception(__FILE__, __LINE__,
                                 "Unsupported SNMP object");
        }
    }
    catch(const std::bad_alloc&)
    {
        throw Exception(__FILE__, __LINE__, err::MEMORYALLOC);
    }
    return res;
}
