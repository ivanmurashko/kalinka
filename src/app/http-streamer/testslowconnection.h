/**
   @file testslowconnection.h
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
   - 2010/11/21 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTSLOWCONNECTION_H
#define KLK_TESTSLOWCONNECTION_H

#include <cppunit/extensions/HelperMacros.h>

#include "teststreamer.h"
#include "testhttpthread.h"

namespace klk
{
    namespace http
    {
        /** @defgroup grTestSlowHTTP Slow connection test
            @{

            There is a test for slow client connection.

            For more info see https://beethoven.lan.spbgasu.ru/trac/ticket/237

            @ingroup grTestHTTP
        */

        /**
           @brief Slow consumer thread

           The thread initiates a connection and don't receive any data
           in a period time more than klk::http::WAITINTERVAL4SLOW

           As result the connection has to be closed by
           the streamer application

        */
        class TestSlowConsumerThread : public test::Thread
        {
        public:
            /// Constructor
            TestSlowConsumerThread();

            /// Destructor
            virtual ~TestSlowConsumerThread(){}
        private:
            /// @copydoc klk::test::Thread::mainLoop
            virtual void mainLoop();
        private:
            /// Fake copy constructor
            TestSlowConsumerThread(const TestSlowConsumerThread&);

            /// Fake assigment operator
            TestSlowConsumerThread& operator=(const TestSlowConsumerThread&);
        };

        /**
           @brief Producer for slow client connection test

           It produces text data with size equal to
           klk::http::CONNECTIONBUFFER_MAX_SIZE/2 + 1 each second. Thus
           after the second iteration we will get connection buffer
           overflow.
        */
        class TestSlowProducerThread : public TestProducerThread
        {
        public:
            /// Constructor
            TestSlowProducerThread();

            /// Destructor
            virtual ~TestSlowProducerThread(){}
        private:
            /// @copydoc klk::test::Thread::mainLoop
            virtual void mainLoop();
        private:
            /// Fake copy constructor
            TestSlowProducerThread(const TestSlowProducerThread&);

            /// Fake assigment operator
            TestSlowProducerThread& operator=(const TestSlowProducerThread&);
        };

        /**
           @brief The slow connection test

           TBD
        */
        class TestSlowConnection : public TestStreamer
        {
            CPPUNIT_TEST_SUITE(TestSlowConnection);
            CPPUNIT_TEST(test);
            CPPUNIT_TEST_SUITE_END();
        public:
            /// Constructor
            TestSlowConnection();

            /// Destructor
            virtual ~TestSlowConnection(){}

            /// The test itself
            void test();

            /// Prepares the test data
            virtual void setUp();
        private:
            /// Fake copy constructor
            TestSlowConnection(const TestSlowConnection&);

            /// Fake assigment operator
            TestSlowConnection& operator=(const TestSlowConnection&);
        };

        /** @} */
    }
}

#endif //KLK_TESTSLOWCONNECTION_H
