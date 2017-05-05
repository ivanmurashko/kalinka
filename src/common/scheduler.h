/**
   @file scheduler.h
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
   - 2008/10/25 created by ipp (Ivan Murashko)
   - 2009/01/01 klk namespace introduced by Ivan Murashko
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_SCHEDULER_H
#define KLK_SCHEDULER_H

#include <list>

#include "ithread.h"
#include "thread.h"

namespace klk
{
    namespace base
    {
        /**
           @brief Thread info class

           ThreadInfo for scheduler
        */
        class ThreadInfo
        {
        public:
            /**
               Constructor
            */
            ThreadInfo(const IThreadPtr& thread, pthread_t id);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            ThreadInfo(const ThreadInfo& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            ThreadInfo& operator=(const ThreadInfo& value);

            /**
               Destructor
            */
            ~ThreadInfo(){}

            /**
               Compare operator
               @param[in] value - the value to be compared

               @return
               - true
               - false
            */
            bool operator==(const ThreadInfo& value) const;

            /**
               Compare operator
               @param[in] thread - the value to be compared

               @return
               - true
               - false
            */
            bool operator==(const IThreadPtr& thread) const;

            /**
               Gets the thread pointer
            */
            const IThreadPtr getThread() const {return m_thread;}

            /**
               Do the join the thread
            */
            void join() const;
        private:
            IThreadPtr m_thread; ///< the thread
            pthread_t m_id; ///< the thread id
        private:
            /**
               Constructor
            */
            ThreadInfo();
        };

        /**
           Container with threads objects

           @ingroup grCommon
        */
        typedef std::list<ThreadInfo> ThreadInfoList;

        /**
           @brief The module factory scheduler

           The scheduler is used for threads handling

           @ingroup grCommon
        */
        class Scheduler
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
               Starts a thread at the scheduler

               @param[in] thread - the thread to be stoped

               @exception klk::Exception
            */
            void startThread(const IThreadPtr& thread);

            /**
               Stops a thread at the scheduler

               @param[in] thread - the thread to be stoped

               @exception klk::Exception
            */
            virtual void stopThread(const IThreadPtr& thread);

            /**
               Checks if the module was already loaded

               @param[in] thread - the thread to be checked

               @return
               - true
               - false
            */
            bool isStarted(const IThreadPtr& thread);

            /**
               Stops all threads
            */
            virtual void stop() throw();

            /**
               Clear stop flag

               @note used in unit tests only
            */
            void clear() {m_stop.init();}
        protected:
            mutable klk::Mutex m_lock; ///< locker mutext

            /**
               Retrives thread info list
            */
            const ThreadInfoList getThreadInfoList() const;
        private:
            ThreadInfoList m_threads; ///< threads
            Trigger m_stop; ///< stop event trigger


            /**
               Retrives the thread list

               @return a copy of the thread list
            */
            const ThreadInfoList getActiveThreads() throw();

            /**
               Checks if the module was already loaded

               @param[in] thread - the thread to be checked

               @note unsafe method

               @return
               - true
               - false
            */
            bool isStartedUnsafe(const IThreadPtr& thread);

            /**
               Stops a thread at the scheduler by means of
               base::ThreadInfo object

               @param[in] info - the info about thread to be stopped

               @exception klk::Exception
            */
            void stopThreadByInfo(const ThreadInfo& info);

            /**
               Retrives thread info by thread

               @param[in] thread - the thread

               @note the result HAS TO be freed by the client

               @return the pointer to the thread info

               @exception klk::Exception if there is no such thread
            */
            ThreadInfo* getThreadInfo(const IThreadPtr& thread);

            /**
               Erases thread info from the threads list

               @param[in] thread - the thread to be erased
            */
            void eraseThread(const IThreadPtr& thread) throw();
        };
    }
}

#endif //KLK_SCHEDULER_H
