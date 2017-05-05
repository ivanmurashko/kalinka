/**
   @file socktest.h
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

#ifndef KLK_SOCKTEST_H
#define KLK_SOCKTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include "testthread.h"
#include "socket/socket.h"
#include "exception.h"

namespace klk
{
    namespace test
    {
        /**
           @brief The test connection thread

           The test connection thread for TCP/IP sockets

           @ingroup grTest
        */
        class TCPConnectThread : public test::Thread
        {
        public:
            /**
               Constructor
            */
            TCPConnectThread();

            /**
               Destructor
            */
            virtual ~TCPConnectThread();

            /**
               The main body of the thread
            */
            virtual void mainLoop();

            /**
               @copydoc IThread::stop
            */
            virtual void stop() throw();
        private:
            klk::ISocketPtr m_sock; ///< socket
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TCPConnectThread(const TCPConnectThread& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TCPConnectThread& operator=(const TCPConnectThread& value);
        };

        /**
           @brief The test connection thread

           The test connection thread for UDP

           @ingroup grTest
        */
        class UDPConnectThread : public test::Thread
        {
        public:
            /**
               Constructor

            */
            UDPConnectThread();

            /**
               Destructor
            */
            virtual ~UDPConnectThread();

            /**
               @copydoc IThread::stop
            */
            virtual void stop() throw();
        private:
            /**
               The main body of the thread
            */
            virtual void mainLoop();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            UDPConnectThread(const UDPConnectThread& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            UDPConnectThread& operator=(const UDPConnectThread& value);
        };

        /**
           @brief The test connection thread

           The test connection thread for Domain (aka unix) sockets

           @ingroup grTest
        */
        class DomainConnectThread : public test::Thread
        {
        public:
            /**
               Constructor
            */
            DomainConnectThread();

            /**
               Destructor
            */
            virtual ~DomainConnectThread();

            /**
               @copydoc IThread::stop
            */
            virtual void stop() throw();
        private:
            klk::ISocketPtr m_sock; ///< socket

            /**
               The main body of the thread
            */
            virtual void mainLoop();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            DomainConnectThread(const DomainConnectThread& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            DomainConnectThread& operator=(const DomainConnectThread& value);
        };


        /**
           @brief The listener thread

           The test listener thread

           The thread works in ech mode (send back the request)

           @ingroup grTest
        */
        class ListenThread : public test::Thread
        {
        public:
            /**
               Constructor

               @param[in] proto - the protocol (TCP/IP or UDP)
            */
            ListenThread(const sock::Protocol proto);

            /**
               Destructor
            */
            virtual ~ListenThread();
        private:
            const sock::Protocol m_proto; ///< protocol
            klk::IListenerPtr m_listener; ///< listener

            /**
               The main body of the thread
            */
            virtual void mainLoop();

            /**
               The stopper
            */
            virtual void stop() throw();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            ListenThread(const ListenThread& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            ListenThread& operator=(const ListenThread& value);
        };

        /**
           @brief The socket unit test

           SocketTest

           @ingroup grTest
        */
        class SocketTest : public CppUnit::TestFixture
        {
            CPPUNIT_TEST_SUITE(SocketTest);
            CPPUNIT_TEST(testTCPIP);
            CPPUNIT_TEST(testUDP);
            CPPUNIT_TEST(testDomain);
            CPPUNIT_TEST_EXCEPTION(test2Connect, klk::Exception);
            CPPUNIT_TEST_SUITE_END();
        public:
            /**
               Constructor
            */
            SocketTest();

            /**
               Destructor
            */
            virtual ~SocketTest();

            /**
               Setups data
            */
            virtual void setUp();

            /**
               Clears data
            */
            virtual void tearDown();

            /**
               Tests the TCPIP sockets
            */
            void testTCPIP();

            /**
               Tests the UDP sockets
            */
            void testUDP();

            /**
               Tests the domain sockets
            */
            void testDomain();

            /**
               Tests the exception after two connect
            */
            void test2Connect();
        private:
            test::Scheduler m_scheduler; ///< the test scheduler

            /**
               Main test

               @param[in] proto - the protocol
            */
            void test(const sock::Protocol proto);
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            SocketTest& operator=(const SocketTest& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            SocketTest(const SocketTest& value);
        };
    }
}

#endif //KLK_SOCKTEST_H
