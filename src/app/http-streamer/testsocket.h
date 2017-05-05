/**
   @file testsocket.h
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

#ifndef KLK_TESTSOCKET_H
#define KLK_TESTSOCKET_H

#include <fstream>

#include "socket/socket.h"

namespace klk
{
    namespace http
    {
        /**
           @brief Test socket

           The class emulates socket operations. The source is a regular file

           @ingroup grTestHTTP
        */
        class TestSocket : public ISocket
        {
        public:
            /**
               Constructor

               @param[in] fname - the file name with data
            */
            TestSocket(const std::string& fname);

            /**
               Destructor
            */
            virtual ~TestSocket();
        private:
            std::ifstream m_file; ///< file itself

            /**
               @copydoc klk::ISocket::getInputRate
            */
            virtual const double getInputRate() const;

            /**
               @copydoc klk::ISocket::getOutputRate
            */
            virtual const double getOutputRate() const;

            /**
               @copydoc klk::ISocket::send
            */
            virtual void send(const BinaryData& data);

            /**
               @copydoc klk::ISocket::recv
            */
            virtual void recv(BinaryData& data);

            /**
               @copydoc klk::ISocket::recvAll
            */
            virtual void recvAll(BinaryData& data);

            /**
               @copydoc klk::ISocket::disconnect
            */
            virtual void disconnect() throw();

            /**
               @copydoc klk::ISocket::checkData
            */
            virtual Result checkData(time_t timeout);

            /**
               @copydoc klk::ISocket::stopCheckData
            */
            virtual void stopCheckData() throw();

            /**
               @copydoc klk::ISocket::getPeerName
            */
            virtual const std::string getPeerName() const throw();

            /**
               @copydoc klk::ISocket::connect
            */
            virtual void connect(const sock::RouteInfo& route);

            /**
               @copydoc klk::ISocket::send
            */
            virtual void
                send(const sock::RouteInfo& route, const BinaryData& data);

            /// @copydoc klk::sock::ISocket::setSendTimeout
            virtual void setSendTimeout(time_t timeout)
            {
            }

            /// @copydoc klk::sock::ISocket::setKeepAlive
            virtual void setKeepAlive(time_t timeout)
            {
            }
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestSocket(const TestSocket& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestSocket& operator=(const TestSocket& value);
        };
    }
}

#endif //KLK_TESTSOCKET_H
