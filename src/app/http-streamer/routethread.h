/**
   @file routethread.h
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
   - 2009/03/09 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_ROUTETHREAD_H
#define KLK_ROUTETHREAD_H

#include <sys/types.h>

#include <string>

#include "thread.h"
#include "httprouteinfo.h"
#include "socket/socket.h"
#include "httpbase.h"

namespace klk
{
    namespace http
    {
        /**
           @brief HTTP base thread

           The base thread is the base class for route input/output threads

           @ingroup grHTTP
        */
        class Thread : public base::Thread, public Base
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory
            */
            Thread(Factory* factory);

            /**
               Destructor
            */
            virtual ~Thread();
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Thread& operator=(const Thread& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Thread(const Thread& value);
        };


        /**
           @brief Route base thread

           The base thread is the base class for route input/output threads

           @ingroup grHttp
        */
        class RouteThread : public Thread
        {
            friend class TestUDP;
        public:
            /**
               Constructor

               @param[in] factory - the factory
            */
            RouteThread(Factory* factory);

            /**
               Destructor
            */
            virtual ~RouteThread();

            /**
               Setups route

               @param[in] info - the info to be set

               @exception klk::Exception
            */
            void setRoute(const RouteInfoPtr& info);

            /**
               Retrives the stored route

               @return the stored route

               @exception klk::Exception in the case of route is NULL
            */
            const RouteInfoPtr getRoute() const;

            /**
               Retrives rate

               @return the rate
            */
            virtual const double getRate() const = 0;

            /**
               Checks is there any route has been already set

               @return
               - true - a route  has been already set
               - false - no route has been set
            */
            const bool checkRoute() const throw();

            /**
               Checks is the route (at the args) was already set

               @param[in] uuid - the uuid to be checked

               @return
               - true - the route with the uuid has been already set
               - false - no route with the uuid has been set
            */
            const bool checkRoute(const std::string& uuid) const throw();
        protected:
            /**
               @copydoc IThread::stop()
            */
            virtual void stop() throw();

            /**
               @copydoc IThread::init()
            */
            virtual void init();

            /**
               Inits listener
            */
            void initListener();

            /**
               resets listener

               free reources

               FIXME!!! is it really necessary?
            */
            void resetListener() throw();

            /**
               Retrives listener pointer

               @return the listener smart pointer

               @exception klk::Exception if the listener was not initialized
            */
            const klk::IListenerPtr getListener() const;
        private:
            IListenerPtr m_listener; ///< listener
            RouteInfoPtr m_route; ///< the route info
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            RouteThread& operator=(const RouteThread& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            RouteThread(const RouteThread& value);
        };
    }
}

#endif //KLK_ROUTETHREAD_H
