/**
   @file udp.h
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

#ifndef KLK_UDPSOCKET_H
#define KLK_UDPSOCKET_H

#include <sys/socket.h>

#include "base.h"
#include "thread.h"

namespace klk
{
    namespace sock
    {
        /** @defgroup grUDPSocket Working with UDP/IP sockets
            @brief Working with UDP/IP sockets

            Classes and functions that are used for working with UDP/IP sockets

            @ingroup grSocket

            One main parameter for UDP sockets is the live time
            The live time is a time in which we try to get data
            if there is not data with the interval the socket
            will be closed

            @{
        */

        /**
           @brief Base UDP socket class

           Base UDP socket class
        */
        class UDPSocket : public Socket
        {
        public:
            /**
               Destructor
            */
            virtual ~UDPSocket(){}
        protected:
            /**
               Constructor
            */
            UDPSocket();

            /**
               Constructor
            */
            UDPSocket(int fd);
        private:
            /**
               Sends data to the specified connection

               @param[in] route - the connection info
               @param[in] data - the data to be sent

               @note Used mainly in UDP socket tests
               FIXME!!! remove it

               @exception klk::Exception
            */
            virtual void send(const RouteInfo& route, const BinaryData& data);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            UDPSocket(const UDPSocket& value);


            /**
               Assigment operator
               @param[in] value - the copy param
            */
            UDPSocket& operator=(const UDPSocket& value);
        };

        /**
           @brief UDP connection socket class

           The class is used to initiate connections to UDP sockets
        */
        class UDPConnectSocket : public UDPSocket
        {
        public:
            /**
               Constructor
            */
            UDPConnectSocket();

            /**
               Destructor
            */
            virtual ~UDPConnectSocket();

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
        private:
            /**
               @copydoc klk::ISocket::getPeerName()

               @note the operation is not valid for UDP sockets. Thus
               it always return a stub value
            */
            virtual const std::string getPeerName() const throw()
            {
                return "n/a";
            }
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            UDPConnectSocket& operator=(const UDPConnectSocket& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            UDPConnectSocket(const UDPConnectSocket& value);
        };


        /**
           @brief UDP listen socket class

           UDP listen socket class
        */
        class UDPListenSocket : public UDPSocket
        {
        public:
            /**
               Constructor

               @param[in] fd - socket descriptor
            */
            UDPListenSocket(int fd);

            /**
               Destructor
            */
            virtual ~UDPListenSocket();

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
               Stops the UDP socket
            */
            void stop() throw();

            /**
               Inits the UDP socket
            */
            void init() {m_event.init();}

            /**
               Starts wait for a socket
            */
            void wait() {m_event.wait();}
        private:
            /// @copydoc klk::ISocket::getPeerName()
            virtual const std::string getPeerName() const throw();

            /**
               Sends a data portion

               @param[in] data - the data to be send

               @exception klk::Exception

               @note FIXME!!! a lot of duplicate code from Socket::send
            */
            virtual void send(const BinaryData& data);
        private:
            Trigger m_event;  ///< event for stopping
            struct sockaddr_in m_addr; ///< last input addr
            socklen_t m_addr_len; ///< last input addr len

            /**
               @brief Closes connection

               Disconnect the socket
            */
            virtual void disconnect() throw();
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            UDPListenSocket& operator=(const UDPListenSocket& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            UDPListenSocket(const UDPListenSocket& value);
        };

        /**
           Smart pointer for UDP socket
        */
        typedef boost::shared_ptr<UDPListenSocket> UDPListenSocketPtr;

        /**
           @brief Listener for UDP connections

           UDPListener
        */
        class UDPListener : public Listener
        {
        public:
            /**
               Constructor

               @param[in] route - the route info
            */
            UDPListener(const RouteInfo& route);

            /**
               Destructor
            */
            virtual ~UDPListener();

        private:
            UDPListenSocketPtr m_current; ///< current socket
            Trigger m_stop; ///< stop trigger

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

            /**
               Bind unicast
            */
            void bind();

            /**
               Join multicast
            */
            void join();

            /**
               Unjoin multicast
            */
            void unjoin() throw();
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            UDPListener& operator=(const UDPListener& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            UDPListener(const UDPListener& value);
        };
        /** @} */
    }
}

#endif //KLK_UDPSOCKET_H
