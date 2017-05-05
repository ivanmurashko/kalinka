/**
   @file testtransthread.h
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
   - 2009/04/02 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTTRANSTHREAD_H
#define KLK_TESTTRANSTHREAD_H

#include "testthread.h"
#include "binarydata.h"
#include "socket/socket.h"

namespace klk
{
    namespace trans
    {
        /**
           @brief The test thread class

           The thread tests transcode data recieved via a socket
           There is a tcp/ip server that accepts connections
           (emulates http streamer input thread)

           @ingroup grTest
        */
        class TestThread : public klk::test::Thread
        {
        public:
            /**
               Constructor
            */
            TestThread();

            /**
               Destructor
            */
            virtual ~TestThread(){}
        private:
            klk::ISocketPtr m_sock; ///< socket for data receive
            IListenerPtr m_listener; ///< listener to accept data
            klk::BinaryData m_data; ///< data recieved

            /**
               @copydoc klk::test::Thread::mainLoop
            */
            virtual void mainLoop();

            /**
               Processing a connection
            */
            void processConnection();

            /**
               Stops the thread
            */
            virtual void stop() throw();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestThread(const TestThread& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestThread& operator=(const TestThread& value);
        };
    }
}

#endif //KLK_TESTTRANSTHREAD_H
