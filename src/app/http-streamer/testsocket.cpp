/**
   @file testsocket.cpp
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
   - 2010/02/22 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <errno.h>

#include <fstream>

#include "testsocket.h"
#include "exception.h"

using namespace klk;
using namespace klk::http;

//
// TestSocket class
//

// Constructor
TestSocket::TestSocket(const std::string& fname) : m_file()
{
    m_file.open(fname.c_str(), std::ifstream::in|std::ifstream::binary);

    if (m_file.fail())
    {
        throw Exception(__FILE__, __LINE__,
                        "Error %d in open(): %s. File name: '%s'",
                        errno, strerror(errno), fname.c_str());
    }
}

// Destructor
TestSocket::~TestSocket()
{
    m_file.close();
}

// @copydoc klk::ISocket::getInputRate
const double TestSocket::getInputRate() const
{
    NOTIMPLEMENTED;
    return 0.0;
}

// @copydoc klk::ISocket::getOutputRate
const double TestSocket::getOutputRate() const
{
    NOTIMPLEMENTED;
    return 0.0;
}

// @copydoc klk::ISocket::send
void TestSocket::send(const BinaryData& data)
{
    NOTIMPLEMENTED;
}

// @copydoc klk::ISocket::recv
void TestSocket::recv(BinaryData& data)
{
    if (!m_file.good())
    {
        // EOF
        data.resize(0);
        return;
    }

    size_t count = m_file.readsome(static_cast<char*>(data.toVoid()), data.size());
    if (count < data.size())
        data.resize(count);
}

// @copydoc klk::ISocket::recvAll
void TestSocket::recvAll(BinaryData& data)
{
    NOTIMPLEMENTED;
}

// @copydoc klk::ISocket::disconnect
void TestSocket::disconnect() throw()
{
    NOTIMPLEMENTED;
}

// @copydoc klk::ISocket::checkData
Result TestSocket::checkData(time_t timeout)
{
    NOTIMPLEMENTED;
    return ERROR;
}

// Stops data check
void TestSocket::stopCheckData() throw()
{
    NOTIMPLEMENTED;
}

// @copydoc klk::ISocket::getPeerName
const std::string TestSocket::getPeerName() const throw()
{
    //NOTIMPLEMENTED;
    return std::string();
}

// @copydoc klk::ISocket::connect
void TestSocket::connect(const sock::RouteInfo& route)
{
    NOTIMPLEMENTED;
}

// @copydoc klk::ISocket::send
void TestSocket::send(const sock::RouteInfo& route, const BinaryData& data)
{
    NOTIMPLEMENTED;
}

