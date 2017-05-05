/**
   @file testhttpthread.h
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
   - 2009/03/06 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTHTTPTHREAD_H
#define KLK_TESTHTTPTHREAD_H

#include "socket/socket.h"
#include "testthread.h"

namespace klk
{
    namespace http
    {
        /**
           Producer thread class

           The thread produces multimedia data
        */
        class TestProducerThread : public test::Thread
        {
        public:
            /**
               Constructor

               @param[in] route - the route the data to be sent
               @param[in] data - the data to be sent
            */
            TestProducerThread(const sock::RouteInfo& route,
                               const std::string& data);

            /**
               Destructor
            */
            virtual ~TestProducerThread();
        protected:
            sock::RouteInfo m_route; ///< the route
            BinaryData m_data; ///< the data
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestProducerThread(const TestProducerThread& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestProducerThread& operator=(const TestProducerThread& value);
        };

        /**
           Consumer thread class

           The thread tests http responses
        */
        class TestConsumerThread : public test::Thread
        {
        public:
            /**
               Constructor
            */
            TestConsumerThread();

            /**
               Destructor
            */
            virtual ~TestConsumerThread();
        private:
            /// @copydoc klk::test::Thread::mainLoop
            virtual void mainLoop();

            /**
               Test head request/response

               @param[in] path - the path on the http streamer

               @exception @ref klk::Exception - there was an error
            */
            void testHead(const std::string& path);

            /**
               Test get request/response

               @param[in] path - the path on the http streamer

               @exception @ref klk::Exception - there was an error
            */
            void testGet(const std::string& path);

            /**
               Test 404 http error

               @exception klk::Exception - there was an error
            */
            void testNotFound();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestConsumerThread(const TestConsumerThread& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestConsumerThread& operator=(const TestConsumerThread& value);
        };
    }
}

#endif //KLK_TESTHTTPTHREAD_H
