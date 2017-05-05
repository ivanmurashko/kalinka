/**
   @file server.cpp
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
   - 2008/07/26 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <memory>

#include "server.h"
#include "common.h"
#include "utils.h"
#include "log.h"
#include "paths.h"
#include "dev.h"
#include "db.h"
#include "exception.h"

// modules specific info
#include "service/defines.h"
#include "adapter/defines.h"

using namespace klk;
using namespace klk::server;

//
// ServerDaemon class
//

// Constructor
ServerDaemon::ServerDaemon(IFactory* factory) :
    m_factory(factory), m_pid(-1), m_lock(-1)
{
    m_pid = getpid();
}

// Destructor
ServerDaemon::~ServerDaemon()
{
}

// starts the server
void ServerDaemon::start()
{
    doStartup();
    klk_log(KLKLOG_INFO, "Kalinka mediaserver started");

    m_factory->getEventTrigger()->wait();

    doStop();
    klk_log(KLKLOG_INFO, "Kalinka mediaserver stopped");
}

// Do startup
void ServerDaemon::doStartup()
{
    lockDaemon();
    startModules();
}



// stops the server
void ServerDaemon::doStop() throw()
{
    klk_log(KLKLOG_INFO, "Kalinka mediaserver stop");

    stopModules();
    unlockDaemon();
}

// Locks the daemon
void ServerDaemon::lockDaemon()
{
    BOOST_ASSERT(m_lock == -1);

    base::Utils::mkParentDir(dir::SERVER_PIDFILE);

    m_lock = open(dir::SERVER_PIDFILE.c_str(), O_WRONLY|O_CREAT|O_TRUNC,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (m_lock < 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Error %d in open(): %s. File name: %s",
                        errno, strerror(errno), dir::SERVER_PIDFILE.c_str());
    }
    if (fchmod(m_lock, S_IRUSR|S_IWUSR) < 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "Error %d in fchmod(): %s",
                        errno, strerror(errno));
    }
    // set write lock
    struct flock lock;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_type = F_WRLCK;

    if (fcntl(m_lock, F_SETLK, &lock) < 0)
    {
        int savederrno = errno;
        klk_close(m_lock);
        m_lock = -1;
        throw Exception(__FILE__, __LINE__,
                        "Error %d while setting lock in fcntl(): %s",
                        savederrno, strerror(savederrno));
    }

    // write the pid
    char buff[128];
    snprintf(buff, sizeof(buff) - 1, "%d", m_pid);
    if (write(m_lock, buff, strlen(buff)) != (int)strlen(buff))
    {
        int savederrno = errno;
        klk_close(m_lock);
        m_lock = -1;
        unlockDaemon();
        throw Exception(__FILE__, __LINE__,
                        "Error %d  in write(): %s",
                        savederrno, strerror(savederrno));
    }

    fsync(m_lock);
}

// Unlocks the daemon
void ServerDaemon::unlockDaemon() throw()
{
    if (m_lock == -1)
        return; // nothing to do

    KLKASSERT(m_lock >= 0);

    // set write lock
    struct flock lock;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_type = F_UNLCK;

    if (fcntl(m_lock, F_SETLK, &lock) < 0)
    {
	klk_log(KLKLOG_ERR,
                "Error %d while setting lock in fcntl(): %s",
                errno, strerror(errno));
        KLKASSERT(0);
        return;
    }

    klk_close(m_lock);
    m_lock = -1;

    base::Utils::unlink(dir::SERVER_PIDFILE);
}

// starts modules for the daemon
void ServerDaemon::startModules()
{
    BOOST_ASSERT(m_factory);
    BOOST_ASSERT(m_factory->getModuleFactory());
    // load adapter always
    m_factory->getModuleFactory()->load(adapter::MODID);
    // load service to provide some generic communication and
    // applications startup
    m_factory->getModuleFactory()->load(srv::MODID);
}

// stops modules for the daemon
void ServerDaemon::stopModules() throw()
{
    try
    {
        m_factory->getModuleFactory()->unloadAll();
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR, "Failed to stop modules: %s",
                err.what());
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Unspecified error");
    }
}
