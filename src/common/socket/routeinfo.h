/**
   @file routeinfo.h
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
   - 2009/03/10 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SOCKETROUTEINFO_H
#define KLK_SOCKETROUTEINFO_H

#include <sys/types.h>

#include <string>

namespace klk
{
    namespace sock
    {
        /**
           Network route type

           @ingroup grSocket
        */
        typedef enum
        {
            UNICAST = 0,
            MULTICAST = 1
        } Type;

        /**
           Network protocol

           @ingroup grSocket
        */
        typedef enum
        {
            TCPIP = 0,
            UDP = 1,
            UNIXDOMAIN = 2
        } Protocol;

        /**
           @brief Route info holder

           The class holds route info used for HTTP streaming

           @ingroup grSocket
        */
        class RouteInfo
        {
        public:
            /**
               Constructor

               @param[in] host - the host
               @param[in] port - the port
               @param[in] proto - the protocol
               @param[in] type - the route type (unicast or multicast)

               The protocol is one of the following
               - sock::UDP
               - sock::TCPIP
               - sock::UNIXDOMAIN
            */
            RouteInfo(const std::string& host, const u_int port,
                      const sock::Protocol proto, const sock::Type type);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            RouteInfo(const RouteInfo& value);

            /**
               Destructor
            */
            virtual ~RouteInfo();

            /**
               Compare operator

               @param[in] value - the value to be compared

               @return
               - true
               - false
            */
            bool operator==(const RouteInfo& value) const throw();

            /**
               Compare operator

               @param[in] value - the value to be compared

               @return
               - true
               - false
            */
            bool operator!=(const RouteInfo& value) const throw();


            /**
               Assigment operator
               @param[in] value - the copy param
            */
            RouteInfo& operator=(const RouteInfo& value);

            /**
               Retrives host info

               @return the host
            */
            const std::string getHost() const throw(){return m_host;}

            /**
               Retrives port info

               @return the port
            */
            const u_int getPort() const throw(){return m_port;}

            /**
               Retrives protocol

               @return the protocol
            */
            const sock::Protocol getProtocol() const throw(){return m_proto;}

            /**
               Retrives type

               @return the type
            */
            const sock::Type getType() const throw(){return m_type;}
        private:
            std::string m_host; ///< the host
            u_int m_port;       ///< the port
            sock::Protocol m_proto; ///< protocol
            sock::Type m_type; ///< the network type
        };
    }
}

#endif //KLK_SOCKETROUTEINFO_H
