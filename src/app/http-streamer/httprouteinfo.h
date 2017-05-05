/**
   @file httprouteinfo.h
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
   - 2009/03/20 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_HTTPROUTEINFO_H
#define KLK_HTTPROUTEINFO_H

#include <list>

#include "mod/info.h"
#include "socket/routeinfo.h"

namespace klk
{
    namespace http
    {
        /**
           @brief Route info container used in HTTP streamer

           Route info container used in HTTP streamer

           @ingroup grHTTP
        */
        class RouteInfo : public mod::Info, public sock::RouteInfo
        {
        public:
            /**
               Constructor

               @param[in] route - the route uuid
               @param[in] name - the route name
               @param[in] host - the host
               @param[in] port - the port
               @param[in] proto - the prorocol
               @param[in] type - the network type
            */
            RouteInfo(const std::string& route,
                      const std::string& name,
                      const std::string& host,
                      const u_int port,
                      const sock::Protocol proto,
                      const sock::Type type);
            /**
               Destructor
            */
            virtual ~RouteInfo();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            RouteInfo(const RouteInfo& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            RouteInfo& operator=(const RouteInfo& value);
        };

        /**
           RouteInfo smart pointer
        */
        typedef boost::shared_ptr<RouteInfo> RouteInfoPtr;
    }
}

#endif //KLK_HTTPROUTEINFO_H
