/**
   @file messageholder.h
   @brief This file is part of Kalinka mediaserver.
   @author ipp <ivan.murashko@gmail.com>

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
   - 2008/10/24 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_MESSAGEHOLDER_H
#define KLK_MESSAGEHOLDER_H

#include <sys/types.h>
#include <pthread.h>

#include <deque>

#include "imessage.h"
#include "common.h"
#include "errors.h"

namespace klk
{
    /**
       Wait timeout for sync response

       @ingroup grThread
    */
#ifdef DEBUG
    const u_long TIMEINTERVAL4SYNCRES = 10;
#else
    const u_long TIMEINTERVAL4SYNCRES = 10;
#endif

    /**
       @brief Base class for message holders

       Base class for message holders

       @ingroup grThread
    */
    class MessageHolderBase
    {
    public:
        /**
           Constructor

           @param[in] interval - the wait interval
        */
        explicit MessageHolderBase(u_long interval);

        /**
           Destructor
        */
        virtual ~MessageHolderBase();

        /**
           Gets a message from the holder

           @param[out] msg - the container for retriving message

           @note the method will wait until @ref stop or @ref add will
           be called

           @return
           - @ref OK
           - @ref ERROR
        */
        Result get(IMessagePtr& msg);

        /**
           Adds a message to the holder

           @param[in] msg - the message to be added

           @note stops waiting in @ref get

           @return
           - @ref OK
           - @ref ERROR
        */
        Result add(const IMessagePtr& msg);

        /**
           Starts processing
           Clears all prev states
        */
        virtual void start();

        /**
           Stops processing
        */
        virtual void stop();
    protected:
        /**
           Gets a message from the holder

           @param[out] msg - the container for retriving message

           @note not thread safe method

           @return
           - @ref OK
           - @ref ERROR
        */
        virtual Result getUnsafe(IMessagePtr& msg) = 0;

        /**
           Adds a message to the holder

           @param[in] msg - the message to be added

           @note not thread safe method

           @return
           - @ref OK
           - @ref ERROR
        */
        virtual Result addUnsafe(const IMessagePtr& msg) = 0;

        /**
           Checks is container empty or not

           @note not thread safe method

           @return
           - true
           - false
        */
        virtual bool empty() = 0;
    private:
        u_long m_wait_interval; ///< wait interval
        bool m_stop; ///< stop flag
        bool m_wait_exit; ///< exit flag
        bool m_wait_loop; ///< was started or not
        pthread_mutex_t m_wait_mutex; ///< wait mutex
        pthread_cond_t m_wait_cond; ///< wait condition
        pthread_condattr_t	m_wait_attr; ///< CondVar attribute

        /**
           locks the object
        */
        void lock();

        /**
           unlocks the object
        */
        void unlock();

        /**
           Constructor
        */
        MessageHolderBase();

        /**
           Copy constructor
           @param[in] value - the copy param
        */
        MessageHolderBase(const MessageHolderBase& value);

        /**
           Assigment operator
           @param[in] value - the copy param
        */
        MessageHolderBase& operator=(const MessageHolderBase& value);
    };

    /**
       @brief Message holder for standard messages

       Message holder for standard messages

       @ingroup grModule
    */
    class MessageHolder4Standard : public MessageHolderBase
    {
    public:
        /**
           Constructor
        */
        MessageHolder4Standard();

        /**
           Destructor
        */
        virtual ~MessageHolder4Standard();

        /**
           Stops processing
        */
        virtual void stop();
    private:
        std::deque<IMessagePtr> m_container; ///< the messages container

        /**
           Checks is container empty or not

           @note not thread safe method

           @return
           - true
           - false
        */
        virtual bool empty() {return m_container.empty();}

        /**
           Gets a message from the holder

           @param[out] msg - the container for retriving message

           @note not thread safe method

           @return
           - @ref OK
           - @ref ERROR
        */
        virtual Result getUnsafe(IMessagePtr& msg);

        /**
           Adds a message to the holder

           @param[in] msg - the message to be added

           @note not thread safe method

           @return
           - @ref OK
           - @ref ERROR
        */
        virtual Result addUnsafe(const IMessagePtr& msg);

        /**
           Copy constructor
           @param[in] value - the copy param
        */
        MessageHolder4Standard(const MessageHolder4Standard& value);

        /**
           Assigment operator
           @param[in] value - the copy param
        */
        MessageHolder4Standard& operator=(const MessageHolder4Standard& value);
    };

    /**
       @brief The holder for sync messages

       The holder for sync messages

       @ingroup grModule
    */
    class MessageHolder4SyncRes : public MessageHolderBase
    {
    public:
        /**
           Constructor
        */
        MessageHolder4SyncRes();

        /**
           Destructor
        */
        virtual ~MessageHolder4SyncRes();

        /**
           Stops processing
        */
        virtual void stop();
    private:
        IMessagePtr m_msg; ///< message

        /**
           Checks is container empty or not

           @note not thread safe method

           @return
           - true
           - false
        */
        virtual bool empty() {return !m_msg;}

        /**
           Gets a message from the holder

           @param[out] msg - the container for retriving message

           @note not thread safe method

           @return
           - @ref OK
           - @ref ERROR
        */
        virtual Result getUnsafe(IMessagePtr& msg);

        /**
           Adds a message to the holder

           @param[in] msg - the message to be added

           @note not thread safe method

           @return
           - @ref OK
           - @ref ERROR
        */
        virtual Result addUnsafe(const IMessagePtr& msg);

        /**
           Copy constructor
        */
        MessageHolder4SyncRes(const MessageHolder4SyncRes&);

        /**
           Assignment operator constructor
        */
        MessageHolder4SyncRes& operator=(const MessageHolder4SyncRes&);
    };
}

#endif //KLK_MESSAGEHOLDER_H
