/**
   @file routeinfo.h
   @brief This file is part of Kalinka mediaserver.
   @author ipp <ivan.murashko@gmail.com>

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
   - 2008/11/07 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_ROUTEINFO_H
#define KLK_ROUTEINFO_H

#include <string>
#include <list>

#include <boost/shared_ptr.hpp>

#include "iresources.h"
#include "ifactory.h"
#include "thread.h"
#include "mod/info.h"


namespace klk
{
    namespace net
    {
        /**
           @brief The route info holder

           The route info holder
        */
        class RouteInfo : public mod::Info
        {
        public:
            /**
               Constructor

               @param[in] route - the route uuid
               @param[in] name  - the route name
               @param[in] host - the route host
               @param[in] port - the route port
               @param[in] protocol - the protocol
               @param[in] type - the protocol's type possible values are
               klk::msg::key::NETUNICAST or klk::msg::key::NETMULTICAST
               @param[in] dev - the route dev
               @param[in] factory - the factory
            */
            RouteInfo(const std::string& route,
                      const std::string& name,
                      const std::string& host,
                      const std::string& port,
                      const std::string& protocol,
                      const std::string& type,
                      const IDevPtr& dev,
                      IFactory* factory);

            /**
               Retrives route host

               @return route's host
            */
            const std::string getHost() const throw();

            /**
               Retrives route port

               @return route's port
            */
            const std::string getPort() const throw();

            /**
               Retrives route protocol

               @return route's protocol
            */
            const std::string getProtocol() const throw();

            /**
               Retrives route type (unicast/multicast)

               @return route's type
            */
            const std::string getType() const throw();


            /**
               Retrives dev info

               @return route's host
            */
            const IDevPtr getDev() const throw();

            /**
               Destructor
            */
            virtual ~RouteInfo(){}

            /**
               Updates inuse info at the db
            */
            void updateInUse();

            /**
               Clears inuse info at the db
            */
            void clearInUse();

            /**
               Tests if the info is used by somebody

               Checks DB to retrive the state

               @return
               - true
               - false
            */
            virtual bool isInUse() throw();

            /**
               Sets in use flag

               Updates DB to set the state

               @param[in] value - the value to be set
            */
            virtual void setInUse(bool value);
        private:
            std::string m_host; ///< host
            std::string m_port; ///< port
            std::string m_protocol; ///< protocol
            std::string m_type; ///< type
            IDevPtr m_dev; ///< network device
            IFactory* const m_factory; ///< factory

            /**
               @copydoc klk::mod::Info::fillOutMessage
            */
            virtual void fillOutMessage(const IMessagePtr& out) const;
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            RouteInfo& operator=(const RouteInfo& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            RouteInfo(const RouteInfo& value);
        };

        /**
           Routes info shared pointer
        */
        typedef boost::shared_ptr<RouteInfo> RouteInfoPtr;

        /**
           Routes list
        */
        typedef std::list<RouteInfoPtr> RouteInfoList;
    }
}

#endif //KLK_ROUTEINFO_H
