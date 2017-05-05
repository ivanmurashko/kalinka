/**
   @file testthread.h
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

#ifndef KLK_TESTTHREAD_H
#define KLK_TESTTHREAD_H

#include <list>
#include <string>

#include <boost/shared_ptr.hpp>

#include "thread.h"
#include "scheduler.h"
#include "errors.h"

namespace klk
{
    namespace test
    {
        /**
           @brief  Test thread

           Test thread base class

           @ingroup grTest
        */
        class Thread : public base::Thread
        {
        public:
            /**
               Constructor
            */
            Thread();

            /**
               Destructor
            */
            virtual ~Thread();

            /**
               Checks the result

               @exception klk::Exception if there was an error
            */
            void checkResult();
        protected:
            klk::Result m_rc; ///< result code

            /**
               Main loop that should be executed
            */
            virtual void mainLoop() = 0;

            /**
               @copydoc klk::IThread::start
            */
            virtual void start();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Thread(const Thread& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Thread& operator=(const Thread& value);
        };

        /**
           Shared pointer for the test thread
        */
        typedef boost::shared_ptr<Thread> ThreadPtr;

        /**
           Container for the test threads
        */
        typedef std::list<ThreadPtr> ThreadList;

        /**
           @brief Test scheduler

           The test scheduler

           @ingroup grTest
        */
        class Scheduler : public base::Scheduler
        {
        public:
            /**
               Constructor
            */
            Scheduler();

            /**
               Destructor
            */
            virtual ~Scheduler();

            /**
               Adds a test thread

               @param[in] thread - the thread to be added
            */
            void addTestThread(const ThreadPtr& thread);

            /**
               Starts all threads
            */
            void start();

            /**
               Checks the result

               @exception klk::Exception if there was an error
            */
            void checkResult();

            /**
               Clears the test thread list
            */
            void clear() throw();
        private:
            ThreadList m_testthreads; ///< producer threads
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            Scheduler(const Scheduler& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            Scheduler& operator=(const Scheduler& value);
        };
    }
}

#endif //KLK_TESTTHREAD_H
