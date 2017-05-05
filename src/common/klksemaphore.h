/**
   @file klksemaphore.h
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
   - 2010/04/27 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_KLKSEMAPHORE_H
#define KLK_KLKSEMAPHORE_H

#include <semaphore.h>
#include <string>
#include "errors.h"

namespace klk
{
    /** @defgroup grSemaphore Semaphores wrappers
        @brief Semaphores wrappers

        There are several classes that provides working with semaphores.
        It's easy to use unnamed semaphores but unfortunatelly
        Darwin (Mac OS X) does not support them. Thus we have both
        realization named and unnamed semaphores.

        @ingroup grThread

        @{
    */

    /**
       @brief Base Semaphore wrapper

       Support operations that are common for both
       named and unnamed semaphores
    */
    class BaseSemaphore
    {
    public:
        /**
           Constructor
        */
        BaseSemaphore();

        /**
           Destructor
        */
        virtual ~BaseSemaphore(){}

        /**
           Inits the semaphore
        */
        virtual void init() = 0;

        /**
           @brief wait call

           It decreases the internal counter and waits until
           it became non negative
        */
        void wait();

        /**
           Waits the specified amount of time

           @param[in] timeout - the specified timeout

           @note If the wait exceed the post signal is
           sent to restore initial state

           @return
           - klk::OK the call was OK
           - klk::ERROR the timeout exceed
        */
        Result timedwait(time_t timeout);

        /**
           @brief Try wait call

           It just descrease the internal counter.

           @note The counter is decreased until it become zero (semaphore is locked)

           @exception klk::Exception if there was an error during the call

           @note the EAGAIN(the counter became zero)
           and EINTR errors do not produce the exception
        */
        void trywait();

        /**
           The wrapper for sem_post call.

           The call increases the internal semaphore counter
        */
        void post();

        /**
           Is the semaphore locked or not

           (has zero count)

           @return
           - true - the semaphore is locked
           - false - the semaphore is not locked
        */
        bool isLocked();
    protected:
        sem_t* m_sem; ///< semaphor handler
    };

    /**
       @brief Named semaphore wrapper

       Used mainly in Darwin
    */
    class NamedSemaphore : public BaseSemaphore
    {
    public:
        /**
           Constructor
        */
        NamedSemaphore();

        /**
           Destructor
        */
        virtual ~NamedSemaphore();

        /**
           @brief Inits the semaphore

           The method recreates the semaphore

           @exception klk::Exception
        */
        virtual void init();
    private:
        std::string m_path; ///< path to the semaphore

        /**
           Creates a semaphore
        */
        void createSemaphore();

        /**
           Destroys the semaphore
        */
        void destroySemaphore() throw();
    private:
        /**
           Assigment operator
           @param[in] value - the copy param
        */
        NamedSemaphore& operator=(const NamedSemaphore& value);

        /**
           Copy constructor
           @param[in] value - the copy param
        */
        NamedSemaphore(const NamedSemaphore& value);
    };

    /**
       @brief UnNamed semaphore wrapper

       Used mainly in Darwin
    */
    class UnNamedSemaphore : public BaseSemaphore
    {
    public:
        /**
           Constructor
        */
        UnNamedSemaphore();

        /**
           Destructor
        */
        virtual ~UnNamedSemaphore();

        /**
           @brief Inits the semaphore

           Clears it to zero state

           it will decrease semaphore count until it will be equal to zero

           @exception klk::Exception
        */
        virtual void init();
    private:
        /**
           Assigment operator
           @param[in] value - the copy param
        */
        UnNamedSemaphore& operator=(const UnNamedSemaphore& value);

        /**
           Copy constructor
           @param[in] value - the copy param
        */
        UnNamedSemaphore(const UnNamedSemaphore& value);
    };

    /**
       Used semaphore type
     */
#ifdef USE_NAMED_SEMAPHORES
    typedef NamedSemaphore Semaphore;
#else
    typedef UnNamedSemaphore Semaphore;
#endif //USE_NAMED_SEMAPHORES

    /** @} */
}

#endif //KLK_KLKSEMAPHORE_H
