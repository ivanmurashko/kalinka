/**
   @file thread.h
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
   - 2008/06/08 created by ipp (Ivan Murashko)
   - 2009/01/01 klk namespace introduced by Ivan Murashko
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_THREAD_H
#define KLK_THREAD_H

#include <sys/types.h>
#include <pthread.h>
#include <signal.h>

#include "common.h"
#include "errors.h"
#include "ithread.h"
#include "klksemaphore.h"

namespace klk
{
    /** @defgroup grThread Working with POSIX threads
        @brief Working with POSIX threads

        Classes and functions that are used for working with POSIX threads

        @ingroup grCommon

        @{
    */

    /**
       @brief Base class for threads

       Implements working with mutexes
    */
    class Mutex
    {
    public:
        /**
           Constructor
        */
        Mutex();

        /**
           Destructor
        */
        ~Mutex();

        /**
           Gets mutex for locking

           @return the mutex
        */
        pthread_mutex_t* getMutex() throw() {return &m_mutex;}
    private:
        pthread_mutex_t m_mutex; ///< the mutex itself
        pthread_mutexattr_t m_attr; ///< attribute for locking
    };

    /**
       @brief Daemon's thread locker

       Daemon's thread locker
    */
    class Locker
    {
    public:
        /**
           Constructor

           @param[in] mutex - the mutex to be locked
        */
        Locker(Mutex* mutex) throw();

        /**
           Destructor

           Unlocks the thread
        */
        ~Locker();
    private:
        pthread_mutex_t *m_mutex; ///< mutex
    };

    /**
       @brief The thread event class

       The class encapsulates working withevents
    */
    class Event
    {
    public:
        /**
           Constructor
        */
        Event();

        /**
           Destructor
        */
        virtual ~Event();

        /**
           Starts wait process

           @param[in] interval - the wait interval

           @return
           - @ref klk::OK the opperation was successful
           - @ref klk::ERROR the operation was failed

           @exception klk::Exception
        */
        klk::Result startWait(u_long interval);

        /**
           Stops waiting
        */
        void stopWait() throw();
    private:
        bool m_exit; ///< exit flag
        bool m_loop; ///< was started or not
        pthread_mutex_t m_wait_mutex; ///< wait mutex
        pthread_cond_t m_wait_cond; ///< wait condition
        pthread_condattr_t	m_wait_attr; ///< CondVar attribute

        /**
           Does a lock
        */
        void lock() throw();

        /**
           Does an unlock
        */
        void unlock() throw();
    };

    /**
       @brief Trigger based on semaphores

       The trigger can be stooped or not
    */
    class Trigger : public ITrigger
    {
    public:
        /**
           Constructor
        */
        Trigger();

        /**
           Destructor
        */
        virtual ~Trigger();

        /**
           @copydoc ITrigger::isStopped
        */
        virtual bool isStopped();

        /**
           @copydoc ITrigger::stop
        */
        virtual void stop() throw();

        /**
           @copydoc ITrigger::wait
        */
        virtual void wait();

        /**
           Inits the trigger

           @exception klk::Exception
        */
        void init();
    private:
        Semaphore m_sem; ///< semaphore wrapper
    private:
        /**
           Assigment operator
           @param[in] value - the copy param
        */
        Trigger& operator=(const Trigger& value);

        /**
           Copy constructor
           @param[in] value - the copy param
        */
        Trigger(const Trigger& value);
    };

    namespace base
    {
        /**
           @brief Thread routins

           The class holds routins that are useful for threads classes
        */
        class Thread : public virtual IThread
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
        protected:
            mutable Mutex m_lock; ///< the locker

            /**
               @copydoc IThread::init
            */
            virtual void init();

            /**
               @copydoc IThread::stop
            */
            virtual void stop() throw();

            /**
               @copydoc IThread::isStopped
            */
            virtual bool isStopped();
        private:
            Trigger m_stop; ///< stop event trigger
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
    }

    /**
       @brief The value wrapper

       The class is a wraper for a value
       that can be got in a thread safe manner
    */
    template<class T> class SafeValue
    {
    public:
        /**
           Default constructor
        */
        SafeValue() : m_lock(), m_value(T())
        {
        }

        /**
           Constructor
        */
        explicit SafeValue(const T& t) :
        m_lock(), m_value(t)
        {
        }

        /**
           Destructor
        */
        ~SafeValue()
        {
        }

        /**
           Sets value

           @param[in] t - the value to be set
        */
        void setValue(const T& t) throw()
        {
            Locker lock(&m_lock);
            m_value = t;
        }

        /**
           Retrives a value

           @return the value
        */
        const T getValue() const throw() {
            Locker lock(&m_lock);
            return m_value;
        }

        /**
           Assignement operator
           @param[in] value - the copy param
        */
        SafeValue& operator=(const T& value) {
            Locker lock(&m_lock);
            m_value = value;

            return *this;
        }

        /**
           Compare operator
        */
        bool operator==(const T& value) const {
            Locker lock(&m_lock);
            return (m_value == value);
        }

        /**
           Convertion to the base type
        */
        operator const T () const {
            Locker lock(&m_lock);
            return m_value;
        }
    private:
        mutable Mutex m_lock; ///< the mutex for protection
        T m_value; ///< value itself
    private:
        /**
           Copy constructor
           @param[in] value - the copy param
        */
        SafeValue(const SafeValue& value);

        /**
           Assigment operator
           @param[in] value - the copy param
        */
        SafeValue& operator=(const SafeValue& value);
    };
    /** @} */
}

#endif //KLK_THREAD_H
