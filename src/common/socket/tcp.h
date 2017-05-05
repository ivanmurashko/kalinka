/**
   @file tcp.h
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

#ifndef KLK_TCPSOCKET_H
#define KLK_TCPSOCKET_H

#include "base.h"

namespace klk
{
    namespace sock
    {

        /** @defgroup grTCPSocket Working with TCP/IP sockets
            @brief Working with TCP/IP sockets

            Classes and functions that are used for working with TCP/IP sockets

            @ingroup grSocket

            @{
        */

        /**
           @brief TCP/IP socket base class

           Base class for TCP/IP socket
        */
        class TCPSocket : public Socket
        {
        public:
            /**
               Constructor
            */
            TCPSocket();

            /**
               Constructor

               @param[in] fd - socket descriptor
            */
            TCPSocket(int fd);

            /**
               Destructor
            */
            virtual ~TCPSocket();

            /**
               Recieves a data portion

               @param[out] data - the data to be recived (filled)

               @exception klk::Exception
            */
            virtual void recv(BinaryData& data);

            /**
               Do the connection

               @param[in] route - the connection info

               @exception klk::Exception
            */
            virtual void connect(const RouteInfo& route);

            /**
               Checks connection

               @param[in] route - the connection info

               @exception klk::Exception

               @return
               - @ref klk::OK the connection is present
               - @ref klk::ERROR - there is not any connection
               to the specified host/port
            */
            static klk::Result checkConnection(const RouteInfo& route);
        private:
            /// @copydoc klk::ISocket::getPeerName()
            virtual const std::string getPeerName() const throw();

            /**
               Sends data to the specified connection

               @param[in] route - the connection info
               @param[in] data - the data to be sent

               @note Used mainly in UDP socket tests
               FIXME!!! remove it

               @exception klk::Exception
            */
            virtual void send(const RouteInfo& route, const BinaryData& data);

            /**
               Check that data can be send i. e. check that socket ready
               to write

               @note timeout for wait is 10 seconds

               @exception klk::Exception if there is no possibility
               to send data
            */
            void checkReady4Send();

            /// @copydoc klk::sock::ISocket::setKeepAlive
            virtual void setKeepAlive(time_t timeout);
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TCPSocket& operator=(const TCPSocket& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TCPSocket(const TCPSocket& value);
        };

        /**
           @brief Listener for TCP/IP connections

           TCPListener
        */
        class TCPListener : public Listener
        {
        public:
            /**
               Constructor

               @param[in] route - the route info
            */
            TCPListener(const RouteInfo& route);

            /**
               Destructor
            */
            virtual ~TCPListener();
        private:
            /**
               Gets sockets

               @return the socket

               @exception klk::Exception
            */
            virtual const ISocketPtr accept();

            /**
               Inits the listener

               @exception klk::Exception
            */
            void init();
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TCPListener& operator=(const TCPListener& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TCPListener(const TCPListener& value);
        };

        /** @} */

    }
}

#endif //KLK_TCPSOCKET_H
