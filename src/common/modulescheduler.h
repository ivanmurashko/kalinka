/**
   @file modulescheduler.h
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
   - 2009/01/09 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_MODULESCHEDULER_H
#define KLK_MODULESCHEDULER_H

#include <boost/function/function0.hpp>

#include "scheduler.h"

namespace klk
{
    /**
       Timer processor functor
    */
    typedef boost::function0<void> TimerFunction;

    /**
       Module scheduler thred list
    */
    typedef std::list<IThreadPtr> ThreadList;

    /**
       @brief Timer thread

       The thread that does an action by a timer

       @ingroup grThread
    */
    class TimerThread : public base::Thread
    {
    public:
        /**
           Constructor

           @param[in] f - the function to be called
           @param[in] intrv - the timer interval
        */
        explicit TimerThread(TimerFunction f,
                             time_t intrv);

        /**
           Destructor
        */
        virtual ~TimerThread(){}
    private:
        TimerFunction m_f; ///< timer function
        time_t m_intrv; ///< timer interval
        klk::Event m_wait; ///< wait object

        /**
           @copydoc IThread::start
        */
        virtual void start();

        /**
           @copydoc IThread::stop
        */
        virtual void stop() throw();
    private:
        /**
           Copy constructor
           @param[in] value - the copy param
        */
        TimerThread(const TimerThread& value);

        /**
           Assigment operator
           @param[in] value - the copy param
        */
        TimerThread& operator=(const TimerThread& value);
    };

    namespace mod
    {
        /**
           @brief Specific scheduler that can be run inside a module

           Specific scheduler that can be run inside a module

           @ingroup grModule
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
            virtual ~Scheduler(){}

            /**
               Adds a thread to be started

               @param[in] thread - the thread to be added
            */
            void addThread(const IThreadPtr& thread);

            /**
               @brief Starts the scheduler

               Starts all threads in the scheduler

               @exception @ref klk::result
            */
            void start();
        private:
            ThreadList m_list; ///< threads
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

#endif //KLK_MODULESCHEDULER_H
