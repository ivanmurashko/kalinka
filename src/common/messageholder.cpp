/**
   @file messageholder.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <string.h>

#include "messageholder.h"
#include "log.h"
#include "exception.h"

using namespace klk;

//
// Class MessageHolderBase
//

// Constructor
// @param[in] interval - the wait interval
MessageHolderBase::MessageHolderBase(u_long interval) :
    m_wait_interval(interval), m_stop(false),
    m_wait_exit(false), m_wait_loop(false)
{
    if (pthread_condattr_init(&m_wait_attr))
    {
        KLKASSERT(0);
        return;
    }

    try
    {
        if (pthread_mutex_init(&m_wait_mutex, NULL))
        {
            throw Exception(__FILE__, __LINE__,
                                 "pthread_mutex_init() failed");
        }

        if (pthread_cond_init(&m_wait_cond, &m_wait_attr))
        {
            throw Exception(__FILE__, __LINE__,
                                 "pthread_cond_init() failed");
        }
    }
    catch(Exception& err)
    {
        KLKASSERT(0);
        pthread_condattr_destroy(&m_wait_attr);
    }


    if (pthread_mutex_init(&m_wait_mutex, NULL))
    {
        KLKASSERT(0);
        pthread_condattr_destroy(&m_wait_attr);
        return;
    }

    if (pthread_cond_init(&m_wait_cond, &m_wait_attr))
    {
        pthread_condattr_destroy(&m_wait_attr);
        return;
    }

    pthread_condattr_destroy(&m_wait_attr);
}

// Destructor
MessageHolderBase::~MessageHolderBase()
{
    pthread_mutex_destroy(&m_wait_mutex);
    pthread_cond_destroy(&m_wait_cond);
}

// locks the object
void MessageHolderBase::lock()
{
    pthread_mutex_lock(&m_wait_mutex);
}

// unlocks the object
void MessageHolderBase::unlock()
{
    pthread_mutex_unlock(&m_wait_mutex);
}


// Gets a message from the holder
// @param[out] msg - the container for retriving message
Result MessageHolderBase::get(IMessagePtr& msg)
{
    Result rc = OK;
    // wait for awhile
    struct timespec to;
    to.tv_sec = time(NULL) + m_wait_interval;
    to.tv_nsec = 0;

    lock();

    if (empty() == true)
    {
        KLKASSERT(m_wait_loop == false);
        while (!m_wait_exit)
        {
            m_wait_loop = true;
            if (m_wait_interval != 0)
            {
                int ret = pthread_cond_timedwait(&m_wait_cond,
                                                 &m_wait_mutex, &to);
                if (/* ( ret == ETIME ) || */( ret == ETIMEDOUT ))
                {
                    rc = ERROR; // timeout exceed
                    break;
                }

                if (ret != EINTR)
                {
                    break;
                }
            }
            else
            {
                pthread_cond_wait(&m_wait_cond, &m_wait_mutex);
            }
        }
    }

    m_wait_loop = false;
    m_wait_exit = false;

    if (m_stop)
    {
        rc = ERROR;
    }
    else
    {
        rc = getUnsafe(msg);
    }

    unlock();
    return rc;
}

// Adds a message to the holder
// @param[in] msg - the message to be added
Result MessageHolderBase::add(const IMessagePtr& msg)
{
    lock();

    Result rc = OK;

    try
    {
        // check exit condition
        if (m_stop)
        {
            // there can be a message
            // (db change) that can be sent to a stopped module
            // thus we should just reject the message
            //KLKASSERT(0);
            throw Exception(__FILE__, __LINE__,
                "Add message attempt was detected during module stop");
        }

        // add it to the container
        if (addUnsafe(msg) != OK)
        {
            //KLKASSERT(0); we have a unit test on it
            throw Exception(__FILE__, __LINE__,
                "Add message failed");
        }

        // stop waiting
        if (m_wait_loop)
        {
            m_wait_exit = true;
            if (pthread_cond_broadcast(&m_wait_cond) != 0)
            {
                klk_log(KLKLOG_ERROR,
                        "Error %d in pthread_cond_broadcast(): %s",
                        errno, strerror(errno));
                throw Exception(__FILE__, __LINE__,
                                     "Error in pthread_cond_broadcast()");
            }
        }
    }
    catch(const std::exception& err)
    {
        //KLKASSERT(0); we have a unit test on it
        klk_log(KLKLOG_ERROR,
                "Error while message add: %s",
                err.what());
        rc = ERROR;
    }

    unlock();

    return rc;
}

// Starts processing
// Clears all prev states
void MessageHolderBase::start()
{
    lock();
    m_stop = false;
    m_wait_loop = false;
    m_wait_exit = false;
    unlock();
}

// Stops processing
void MessageHolderBase::stop()
{
    lock();

    m_stop = true;

    // stop waiting
    if (m_wait_loop)
    {
        m_wait_exit = true;
        if (pthread_cond_broadcast(&m_wait_cond) != 0)
        {
            klk_log(KLKLOG_ERROR,
                    "Error %d in pthread_cond_broadcast(): %s",
                    errno, strerror(errno));
            KLKASSERT(0);
        }
    }
    unlock();
}

//
// MessageHolder4Standard class
//

// Constructor
MessageHolder4Standard::MessageHolder4Standard() :
    MessageHolderBase(0)
{
}

// Destructor
MessageHolder4Standard::~MessageHolder4Standard()
{
}

// Gets a message from the holder
Result MessageHolder4Standard::getUnsafe(IMessagePtr& msg)
{
    if (m_container.empty())
    {
        return ERROR;
    }

    msg = m_container.front();
    CHECKNOTNULL(msg);
    m_container.pop_front();
    return OK;
}

// Adds a message to the holder
// @param[in] msg - the message to be added
Result MessageHolder4Standard::addUnsafe(const IMessagePtr& msg)
{
    CHECKNOTNULL(msg);
    m_container.push_back(msg);
    return OK;
}

// Stops processing
void MessageHolder4Standard::stop()
{
    // check that container is not empty
    if (m_container.empty() == false)
    {
        klk_log(KLKLOG_ERROR,
                "There are %d unprocessed messages at container",
                m_container.size());
    }

    m_container.clear();

    MessageHolderBase::stop();
}

//
// SyncMessageHolder4SyncRes class
//

// Constructor
MessageHolder4SyncRes::MessageHolder4SyncRes() :
    MessageHolderBase(TIMEINTERVAL4SYNCRES)
{
}

// Destructor
MessageHolder4SyncRes::~MessageHolder4SyncRes()
{
}

// Gets a message from the holder
// @param[out] msg - the container for retriving message
// @note not thread safe method
Result MessageHolder4SyncRes::getUnsafe(IMessagePtr& msg)
{
    msg.reset();
    if (!m_msg)
    {
        return ERROR;
    }
    msg = m_msg;
    m_msg.reset();
    return OK;
}

// Adds a message to the holder
// @param[in] msg - the message to be added
// @note not thread safe method
Result MessageHolder4SyncRes::addUnsafe(const IMessagePtr& msg)
{
    BOOST_ASSERT(msg->getType() == msg::SYNC_RES);
    BOOST_ASSERT(msg);
    m_msg = msg;
    return OK;
}

// Stops processing
void MessageHolder4SyncRes::stop()
{
    if (m_msg)
    {
        klk_log(KLKLOG_ERROR, "Unprocessed sync responses");
        m_msg.reset();
    }

    MessageHolderBase::stop();
}
