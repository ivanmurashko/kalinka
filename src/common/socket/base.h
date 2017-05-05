/**
   @file base.h
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

#ifndef KLK_BASESOCKET_H
#define KLK_BASESOCKET_H

#include "socket.h"
#include "rater.h"
#include "thread.h"

namespace klk
{
    namespace sock
    {
        /**
           @brief Raw socket data

           Raw

           @ingroup grSocket
        */
        class Raw
        {
        public:
            /**
               Constructor
            */
            Raw();

            /**
               Constructor

               @param[in] sock - the socket descriptor value
            */
            Raw(int sock);

            /**
               Destructor
            */
            virtual ~Raw();

            /**
               Checks is there any data available at the socket or not

               @param[in] timeout - the timeout interval during that we wait
               for the data

               @return
               - @ref klk::OK - data available
               - @ref klk::ERROR - no data or timeout

               @exception klk::Exception
            */
            klk::Result checkData(time_t timeout);

            /**
               Stops data check
            */
            void stopCheckData() throw();

            /**
               @brief Closes connection

               Disconnect the socket
            */
            void disconnect() throw();

            /**
               Retrives descriptor
            */
            const int getDescriptor() const throw();

            /**
               Sets descriptor
            */
            void setDescriptor(int sock);
        private:
            int m_sock; ///< socket descriptor
            int m_stop[2]; ///< stop fds

            mutable Mutex m_lock; ///< locker
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Raw& operator=(const Raw& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Raw(const Raw& value);
        };

        /**
           @brief The base socket class

           The base socket class

           @ingroup grSocket
        */
        class Socket : public ISocket
        {
        public:
            /**
               Destructor
            */
            virtual ~Socket();

            /**
               @brief Closes connection

               Disconnect the socket
            */
            virtual void disconnect() throw();

            /**
               Checks is there any data available at the socket or not

               @param[in] timeout - the timeout interval during that we wait
               for the data. Negative value means infinity

               @return
               - @ref klk::OK - data available
               - @ref klk::ERROR - no data or timeout

               @exception klk::Exception
            */
            virtual Result checkData(time_t timeout);

            /**
               Stops data check
            */
            virtual void stopCheckData() throw();

            /**
               Helper function for host by name

               @param[in] route - the host info
               @param[out] addr - the result

               @exception klk::Exception
            */
            static void gethostbyname(const RouteInfo& route,
                                      struct sockaddr_in& addr);

            /**
               Sends a data portion

               @param[in] data - the data to be send

               @exception klk::Exception
            */
            virtual void send(const BinaryData& data);
        protected:
            Raw m_sock; ///< the socket descriptor
            Rater m_rater; ///< rate mesure

            /**
               Default constructor
            */
            Socket();

            /**
               Constructor from fd
            */
            Socket(int fd);

            /**
               Converts struct sockaddr_in into human readable
               form

               @param[in] af - address family (AF_INET or AF_INET6)
               @param[in] addr - the addr to be converted

               @return the address in human readable form
            */
            static const std::string
                addr2String(const int af,
                            const struct sockaddr_in& addr);
        private:
            /**
               Retrive input rate

               @return the input data rate
            */
            virtual const double getInputRate() const;

            /**
               Retrive output rate

               @return the output data rate
            */
            virtual const double getOutputRate() const;

            /**
               Recieves all required data portion

               @param[out] data - the data to be recived (filled)

               @exception klk::Exception
            */
            virtual void recvAll(BinaryData& data);

            /// @copydoc klk::sock::ISocket::setSendTimeout
            virtual void setSendTimeout(time_t timeout);

            /// @copydoc klk::sock::ISocket::setKeepAlive
            virtual void setKeepAlive(time_t timeout);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Socket(const Socket& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Socket& operator=(const Socket& value);
        };

        /**
           @brief Base listener class

           Listener - the base listener class
        */
        class Listener : public IListener
        {
        public:
            /**
               Constructor
            */
            Listener(const RouteInfo& route);

            /**
               Destructor
            */
            virtual ~Listener();
        protected:
            RouteInfo m_route; ///< route
            Raw m_sock; ///< socket

            /**
               Updates socket info after bind
            */
            void updateRouteInfo();

            /**
               Disconnects the socket
            */
            void disconnect() throw();

            /**
               Stops accept in the listener
            */
            virtual void stop() throw(){m_sock.stopCheckData();}
        private:
            /**
               Retrives route info

               @return the route info
            */
            virtual const RouteInfo getRouteInfo() const {return m_route;}
        };
    }
}

#endif //KLK_BASESOCKET_H
