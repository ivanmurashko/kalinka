/**
   @file socket.h
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
   - 2009/03/07 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SOCKET_H
#define KLK_SOCKET_H

#include <sys/types.h>
#include <time.h>

#include <string>

#include <boost/shared_ptr.hpp>

#include "binarydata.h"
#include "errors.h"
#include "routeinfo.h"

namespace klk
{

    /** @defgroup grSocket Working with sockets
        @brief Working with sockets

        Classes and functions that are used for working with sockets

        @ingroup grCommon

        @{
    */

    /**
       @brief Socket interface

       Socket interface
    */
    class ISocket
    {
    public:
        /**
           Destructor
        */
        virtual ~ISocket(){}

        /**
           Retrive input rate

           @return the input data rate
        */
        virtual const double getInputRate() const = 0;

        /**
           Retrive output rate

           @return the output data rate
        */
        virtual const double getOutputRate() const = 0;

        /**
           Sends a data portion

           @param[in] data - the data to be send

           @exception Exception
        */
        virtual void send(const BinaryData& data) = 0;

        /**
           Recieves a data portion

           @param[out] data - the data to be recived (filled)

           @exception Exception
        */
        virtual void recv(BinaryData& data) = 0;

        /**
           Recieves all required data portion

           @param[out] data - the data to be recived (filled)

           @exception Exception
        */
        virtual void recvAll(BinaryData& data) = 0;

        /**
           @brief Closes connection

           Disconnect the socket
        */
        virtual void disconnect() throw() = 0;

        /**
           Checks is there any data available at the socket or not

           @param[in] timeout - the timeout interval during that we wait
           for the data

           @return
           - @ref OK - data available
           - @ref ERROR - no data or timeout

           @exception Exception
        */
        virtual Result checkData(time_t timeout) = 0;

        /**
           Stops data check
        */
        virtual void stopCheckData() throw() = 0;

        /**
           Retrives peername

           @return the name

           @note the function should not throw exception
           because it can be used during special exception creation
           in the case of error the function will return an
           'unknown' idetifier
        */
        virtual const std::string getPeerName() const throw() = 0;

        /**
           Do the connection

           @param[in] route - the connection info

           @exception Exception
        */
        virtual void connect(const sock::RouteInfo& route) = 0;

        /**
           Sends data to the specified connection

           @param[in] route - the connection info
           @param[in] data - the data to be sent

           @note Used mainly in UDP socket tests
           FIXME!!! remove it

           @exception Exception
        */
        virtual void
            send(const sock::RouteInfo& route, const BinaryData& data) = 0;

        /**
           Sets timeout for send() in seconds. 0 means no timeout

           @param[in] timeout - the timeout to be set
        */
        virtual void setSendTimeout(time_t timeout) = 0;

        /**
           Sets keepalive socket parameter

           @param[in] timeout - The time (in seconds) the
           connection needs to remain idle before TCP starts
           sending keepalive probes. 0 means disable the keepalive
        */
        virtual void setKeepAlive(time_t timeout) = 0;
    };

    /**
       The shared pointer for socket class
    */
    typedef boost::shared_ptr<ISocket> ISocketPtr;

    /**
       @brief Listener interface

       IListener listener interface
    */
    class IListener
    {
    public:
        /**
           Destructor
        */
        virtual ~IListener(){}

        /**
           Gets sockets

           @return the socket

           @exception Exception
        */
        virtual const ISocketPtr accept() = 0;

        /**
           Retrives route info

           @return the route info
        */
        virtual const sock::RouteInfo getRouteInfo() const = 0;

        /**
           Stops accept in the listener
        */
        virtual void stop() throw() = 0;
    };

    /**
       The shared pointer for listener class
    */
    typedef boost::shared_ptr<IListener> IListenerPtr;

    namespace sock
    {
        /**
           @brief Socket factory

           Socket factory

           @ingroup grSocket
        */
        class Factory
        {
        public:
            /**
               Constructor
            */
            Factory(){}

            /**
               Destructor
            */
            virtual ~Factory(){}

            /**
               Creates listener from route info

               @param[in] route - the route info that will be used
               for listener creation
            */
            static const IListenerPtr getListener(const sock::RouteInfo& route);

            /**
               Creates socket specified for a protocol

               @param[in] proto - the protocol
            */
            static const ISocketPtr getSocket(const sock::Protocol proto);

            /**
               Checks connection

               @param[in] route - the connection info

               @exception Exception

               @return
               - @ref OK the connection is present
               - @ref ERROR - there is not any connection
               to the specified host/port
            */
            static Result checkConnection(const sock::RouteInfo& route);
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Factory& operator=(const Factory& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Factory(const Factory& value);
        };
    }

    /**
       The socket buffer size
    */
    const size_t SOCKBUFFSIZE = (16384);

    /** @} */
};

#endif //KLK_SOCKET_H
