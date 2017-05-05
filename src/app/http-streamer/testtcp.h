/**
   @file testtcp.h
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
   - 2009/05/21 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTTCP_H
#define KLK_TESTTCP_H

#include <cppunit/extensions/HelperMacros.h>

#include "teststreamer.h"
#include "testhttpthread.h"

namespace klk
{
    namespace http
    {
        /**
           Producer thread class for TCP/IP connection

           The thread produces multimedia data

           @ingroup grTestHTTP
        */
        class TestProducerThread4TCP : public TestProducerThread
        {
        public:
            /**
               Constructor

               @param[in] route - the route the data to be sent
               @param[in] data - the data to be sent
            */
            TestProducerThread4TCP(const sock::RouteInfo& route,
                                   const std::string& data);

            /**
               Destructor
            */
            virtual ~TestProducerThread4TCP();
        private:
            /// @copydoc klk::test::Thread::mainLoop
            virtual void mainLoop();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestProducerThread4TCP(const TestProducerThread4TCP& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestProducerThread4TCP& operator=(const TestProducerThread4TCP& value);
        };

        /**
           @brief TCP connections unit test

           The TCP connections  unit test for HTTP streamer

           The test starts 2 producer threads that provides
           simple text data for HTTP streamer. The data
           assigned to two different paths.

           There are also several consumer threads that tests the
           data on different path by means of HTTP GET and HEAD requests

           @ingroup grTest

           @see klkhttp::TestUDP
        */
        class TestTCP : public TestStreamer
        {
            CPPUNIT_TEST_SUITE(TestTCP);
            CPPUNIT_TEST(test);
            CPPUNIT_TEST_SUITE_END();
        public:
            /**
               Constructor
            */
            TestTCP();

            /**
               Destructor
            */
            virtual ~TestTCP(){}

            /**
               Tests main loop for TCP/IP connections
            */
            void test();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestTCP(const TestTCP& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestTCP& operator=(const TestTCP& value);
        };
    }
}

#endif //KLK_TESTTCP_H