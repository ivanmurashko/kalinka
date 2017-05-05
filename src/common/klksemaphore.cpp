/**
   @file klksemaphore.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <boost/assert.hpp>

#include "klksemaphore.h"
#include "utils.h"
#include "exception.h"
#include "log.h"

using namespace klk;

//
// BaseSemaphore class
//

// Constructor
BaseSemaphore::BaseSemaphore() : m_sem(NULL)
{
}

// wait call
void BaseSemaphore::wait()
{
    int rc = sem_wait(m_sem);
    if (rc < 0)
    {
        switch (errno)
        {
        case EINTR:
            break;
        default:
            throw Exception(__FILE__, __LINE__,
                            "Error %d in sem_wait(): %s",
                            errno, strerror(errno));
        }
    }
}

// Waits the specified amount of time
Result BaseSemaphore::timedwait(time_t timeout)
{
    BOOST_ASSERT(timeout > 0);
    timespec tv;
    tv.tv_sec = time(NULL) + timeout;
    tv.tv_nsec = 0;
    Result result = klk::OK;
#ifdef DARWIN
    // sem_timedwait is not supported on Mac OS X :(
    int rc = sem_wait(m_sem);
#else
    int rc = sem_timedwait(m_sem, &tv);
#endif
    if (rc < 0)
    {
        switch (errno)
        {
        case EINTR:
        case ETIMEDOUT:
            // restore initial state
            post();
            break;
        default:
            throw Exception(__FILE__, __LINE__,
                            "Error %d in sem_wait(): %s",
                            errno, strerror(errno));
        }
        result = klk::ERROR;
    }
    return result;
}


// wait call
void BaseSemaphore::trywait()
{
    int rc = sem_trywait(m_sem);
    if (rc < 0)
    {
        switch (errno)
        {
        case EINTR:
        case EAGAIN:
            break;
        default:
            throw Exception(__FILE__, __LINE__,
                            "Error %d in sem_trywait(): %s",
                            errno, strerror(errno));
        }
    }
}


// post call
void BaseSemaphore::post()
{
    if (sem_post(m_sem) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Error %d in sem_post(): %s",
                        errno, strerror(errno));
    }
}

// Is the semaphore locked or not
bool BaseSemaphore::isLocked()
{
    int rc = sem_trywait(m_sem);
    if (rc < 0)
    {
        switch (errno)
        {
        case EAGAIN:
            break;
        default:
            throw Exception(__FILE__, __LINE__,
                            "Error %d in sem_trywait(): %s",
                            errno, strerror(errno));
        }
        return true;
    }

    // restore prev state
    post();

    return false;
}

//
// NamedSemaphore class
//

// Constructor
NamedSemaphore::NamedSemaphore() :
    BaseSemaphore(), m_path()
{
    createSemaphore();
}

// Destructor
NamedSemaphore::~NamedSemaphore()
{
    destroySemaphore();
}

// Creates a semaphore
void NamedSemaphore::createSemaphore()
{
    KLKASSERT(m_path.empty());
    // FIXME!!! bad code
    std::vector<char> buffer_storage(31);
    char* buffer = &buffer_storage[0];
    strncpy(buffer, "/tmp/klksemXXXXXXX", buffer_storage.size() - 1);
    m_path = mktemp(buffer);
    m_sem = sem_open(m_path.c_str(), O_RDWR | O_CREAT, 0600, 0);
    if (m_sem == static_cast<sem_t*>(SEM_FAILED))
    {
        klk_log(KLKLOG_ERROR, "Error %d in sem_open(): %s",
                errno, strerror(errno));
        KLKASSERT(0);
    }
}

// Destroys the semaphore
void NamedSemaphore::destroySemaphore() throw()
    try
    {
        if (m_sem)
        {
            if (sem_close(m_sem) < 0)
            {
                throw Exception(__FILE__, __LINE__,
                                "Error %d in sem_close(): %s",
                                errno, strerror(errno));
            }
            m_sem = NULL;
        }
        if (!m_path.empty())
        {
            if (sem_unlink(m_path.c_str()) < 0)
            {
                throw Exception(__FILE__, __LINE__,
                                "Error %d in sem_unlink(): %s",
                                errno, strerror(errno));

            }
            m_path.clear();
        }
    }
    catch(...)
    {
        // critical error
        KLKASSERT(0);
    }

// Inits the semaphore
void NamedSemaphore::init()
{
    // recreate the semaphore
    destroySemaphore();
    createSemaphore();
}

//
// UnNamedSemaphore class
//

// Constructor
UnNamedSemaphore::UnNamedSemaphore() :
    BaseSemaphore()
{
    m_sem = new sem_t;
    if (sem_init(m_sem, 0, 0) < 0)
    {
        klk_log(KLKLOG_ERROR, "Error %d in sem_init(): %s",
                errno, strerror(errno));
        KLKASSERT(0);
    }
}

// Destructor
UnNamedSemaphore::~UnNamedSemaphore()
{
    if (m_sem)
    {
        if (sem_destroy(m_sem) < 0)
        {
            klk_log(KLKLOG_ERROR, "Error %d in sem_destroy(): %s",
                    errno, strerror(errno));
            KLKASSERT(0);
        }
        KLKDELETE(m_sem);
    }
}

// Inits the semaphore
void UnNamedSemaphore::init()
{
    if (sem_init(m_sem, 0, 0) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Error %d in sem_init(): %s",
                        errno, strerror(errno));
    }
}

