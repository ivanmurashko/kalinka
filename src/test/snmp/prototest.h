/**
   @file prototest.h
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
   - 2009/05/30 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_PROTOTEST_H
#define KLK_PROTOTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include "testthread.h"
#include "socket/socket.h"

namespace klk
{
    namespace snmp
    {
        /**
           @brief The test thread for the protocol

           The test thread for SNMP protocol utest

           @ingroup grTest
        */
        class ProtocolTestThread : public klk::test::Thread
        {
        public:
            /**
               Constructor
            */
            ProtocolTestThread();

            /**
               Destructor
            */
            virtual ~ProtocolTestThread();
        private:
            klk::IListenerPtr m_listener; ///< listener

            /**
               The main body of the thread
            */
            virtual void mainLoop();

            /**
               @copydoc IThread::stop
            */
            virtual void stop() throw();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            ProtocolTestThread(const ProtocolTestThread& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            ProtocolTestThread& operator=(const ProtocolTestThread& value);
        };

        /**
           @brief Protocol protocol unit test

           Protocol unit test

           @ingroup grTest
        */
        class ProtocolTest : public CppUnit::TestFixture
        {
            CPPUNIT_TEST_SUITE(ProtocolTest);
            CPPUNIT_TEST(test);
            CPPUNIT_TEST_SUITE_END();
        public:
            /**
               Constructor
            */
            ProtocolTest();

            /**
               Destructor
            */
            virtual ~ProtocolTest(){}

            /**
               Constructor
            */
            virtual void setUp();

            /**
               Destructor
            */
            virtual void tearDown();

            /**
               Do the test
            */
            void test();
        private:
            test::Scheduler m_scheduler; ///< the test scheduler

            /**
               Tests a scalar
            */
            void testScalar();

            /**
               Tests a table
            */
            void testTable();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            ProtocolTest(const ProtocolTest& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            ProtocolTest& operator=(const ProtocolTest& value);
        };
    }
}

#endif //KLK_PROTOTEST_H
