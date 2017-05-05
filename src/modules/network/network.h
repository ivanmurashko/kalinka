/**
   @file network.h
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
   - 2008/08/22 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_NETWORK_H
#define KLK_NETWORK_H

#include "modulewithinfo.h"
#include "routeinfo.h"
#include "thread.h"

namespace klk
{
    namespace net
    {
        /**
           @brief The network module main class implementation

           The Network class implementation

           @ingroup grNetModule
        */
        class Network : public klk::ModuleWithInfo<RouteInfo>
        {
            friend class TestNetwork;
        public:
            /**
               Constructor

               @param[in] factory the module factory
            */
            Network(IFactory *factory);

            /**
               Destructor
            */
            virtual ~Network();

            /**
               Retrives the route info list

               @return the list
            */
            const RouteInfoList getRouteInfo() const;

            /**
               Checks is the host multicast or not

               @param[in] host - the host to be checked

               @return
               - true (multicast)
               - false (unicast)
            */
            static const bool isMulticast(const std::string& host);
        private:
            /**
               Process changes at the DB for a specified dev

               @param[in] dev - the dev to be processed
               @param[in] result - the result container

               @exception @ref klk::Exception
            */
            void processDev(const IDevPtr& dev, InfoSet& result);

            /**
               Register all processors

               @exception @ref klk::Exception
            */
            virtual void registerProcessors();

            /**
               Gets a human readable module name

               @return the name
            */
            virtual const std::string getName() const throw();

            /**
               Do the start

               @param[in] in - the input message
               @param[out] out - the output message

               @exception @ref klk::Exception
            */
            virtual void doStart(const IMessagePtr& in,
                                 const IMessagePtr& out);

            /**
               @copydoc IModule::getType
            */
            virtual mod::Type getType() const throw() {return mod::MOD;}

            /**
               Do the list command
               that retrives available routes

               @param[in] in - the input message
               @param[out] out - the output message

               @exception @ref klk::Exception
            */
            void doList(const IMessagePtr& in,
                        const IMessagePtr& out);

            /**
               Retrives info about a route
               without locking it. This is usefull for client requests
               because there are a lot of clients that can use one route

               @param[in] in - the input message
               @param[out] out - the output message

               @exception @ref klk::Exception
            */
            void doInfo(const IMessagePtr& in,
                        const IMessagePtr& out);

            /**
               Retrives an list with data from @ref grDB "database"

               @return list
            */
            virtual const InfoSet getInfoFromDB();

            /**
               Updates lock for locked info
            */
            void updateLock();

            /**
               Do some actions after main loop
            */
            virtual void postMainLoop() throw();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Network(const Network& value);

            /**
               Assignment opearator
            */
            Network& operator=(const Network&);
        };
    }
}

#endif //KLK_NETWORK_H

