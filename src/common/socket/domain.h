/**
   @file domain.h
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

#ifndef KLK_DOMAIN_H
#define KLK_DOMAIN_H

#include <sys/socket.h>
#include <sys/un.h>

#include "tcp.h"

namespace klk
{
    namespace sock
    {

        /** @defgroup grDomainSocket Working with domain sockets
            @brief Working with domain sockets

            Domain (aka local, unix, file) socket are the fastest sockets that
            communicates via a file

            There are classes and files for working with such sockets

            We consider stream sockets @see grTCPIPSocket

            @ingroup grSocket

            @{
        */


        /**
           @brief Domain socket base class

           Base class for Domain socket
        */
        class DomainSocket : public TCPSocket
        {
        public:
            /**
               Constructor
            */
            DomainSocket();

            /**
               Constructor

               @param[in] fd - socket descriptor
            */
            DomainSocket(int fd);

            /**
               Destructor
            */
            virtual ~DomainSocket();

            /**
               Do the connection

               @param[in] route - the connection info

               @exception klk::Exception
            */
            virtual void connect(const RouteInfo& route);

            /**
               Helper function for host by name

               @param[in] route - the route info
               @param[out] addr - the result
               @param[out] size - the result size

               @exception klk::Exception
            */
            static void gethostbyname(const RouteInfo& route,
                                      struct sockaddr_un& addr,
                                      size_t& size);

            /**
               Checks connection

               @param[in] route - the connection info

               @exception klk::Exception

               @return
               - @ref klk::OK the connection is present
               - @ref klk::ERROR - there is not any connection
               to the specified host/port
            */
            static Result checkConnection(const RouteInfo& route);
        private:
            /// @copydoc klk::ISocket::getPeerName()
            virtual const std::string getPeerName() const throw();
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            DomainSocket& operator=(const DomainSocket& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            DomainSocket(const DomainSocket& value);
        };

        /**
           @brief Listener for Domain/IP connections

           DomainListener
        */
        class DomainListener : public Listener
        {
        public:
            /**
               Constructor

               @param[in] route - the route info
            */
            DomainListener(const RouteInfo& route);

            /**
               Destructor
            */
            virtual ~DomainListener();
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

            /**
               Stops accept in the listener
            */
            virtual void stop() throw();
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            DomainListener& operator=(const DomainListener& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            DomainListener(const DomainListener& value);
        };
        /** @} */
    }
}

#endif //KLK_DOMAIN_H
