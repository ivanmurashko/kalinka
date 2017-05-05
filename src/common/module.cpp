/**
   @file module.cpp
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
   - 2008/10/18 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <errno.h>
#include <sys/resource.h>

#include <mysql.h>

#include <boost/bind.hpp>

#include "module.h"
#include "common.h"
#include "log.h"
#include "thread.h"
#include "cli.h"
#include "commontraps.h"
#include "version.h"

// modules specific info
#include "msgcore/defines.h"
#include "adapter/defines.h"
#include "exception.h"

using namespace klk;

//
// Module
//

// Constructor
Module::Module(IFactory *factory,
               const std::string& id) :
    base::Thread(), m_factory(factory),
    m_id(id),
    m_container(), m_sync_container(),
    m_processor(factory, id),
    m_start_time(time(NULL)),
    m_scheduler(),
    m_usage(),
    m_start_sem(),
    m_checkpoint_count(0),
    m_checkpoint_mutex()
{
    BOOST_ASSERT(m_factory);
}

// Gets factory
IFactory* Module::getFactory()
{
    BOOST_ASSERT(m_factory);
    return m_factory;
}

// Do some actions before main loop
void Module::preMainLoop()
{
    m_start_time = time(NULL);
    // On some platforms RUSAGE_THREAD can be defines
    // but can not be used see ticket #187
#ifdef RUSAGE_THREAD
    try
    {
        m_usage = UsagePtr(new Usage(RUSAGE_THREAD));
    }
    catch(...)
#endif //RUSAGE_THREAD
    {
        m_usage = UsagePtr(new Usage());
    }
    // starts threads
    m_scheduler.start();
}

// Do some actions after main loop
void Module::postMainLoop() throw()
{
    m_start_time = 0;
    m_sync_container.stop();
    // stop threads
    m_scheduler.stop();
}

// Starts main loop (processing) in the module
void Module::start()
{
    try
    {
        preMainLoop();

        // initial startup was done
        passStartupCheckpoint();

        klk_log(KLKLOG_DEBUG, "Starting main loop for module '%s'",
                getName().c_str());
        while (isStopped() == false)
        {
            processMessage();
        }
        klk_log(KLKLOG_DEBUG, "Main loop for module '%s' finished",
                getName().c_str());
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR,
                "Got an exception during main loop execution. "
                "Module name: '%s'; Description: %s",
                getName().c_str(),
                err.what());
        postMainLoop();
        throw;
    }
    catch(...)
    {
        postMainLoop();
        throw Exception(__FILE__, __LINE__,
                             "Got an unknown exception during "
                             "main loop execution. Module name: " +
                             getName());
    }
    postMainLoop();
}

// Processes a message
void Module::processMessage() throw()
{
    try
    {
        IMessagePtr msg;
        if (m_container.get(msg) != OK)
        {
            if (!isStopped())
            {
                klk_log(
                    KLKLOG_ERROR,
                    "Error while message retreiving at module '%s'",
                    getName().c_str());
            }
            return;
        }
        m_processor.process(msg);
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR,
                "Got an exception during a message processing. "
                "Module name: '%s'; Description: %s",
                getName().c_str(),
                err.what());
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR,
                "Got an unknown exception during a message processing. "
                "Module name: '%s'",
                getName().c_str());
    }
}


// Does pre actions before start main loop
void Module::init()
{
    Locker lock(&m_lock);
    Thread::init();
    BOOST_ASSERT(isStopped() == false);
    m_container.start();
    m_sync_container.start();

    m_start_sem.init();
    registerStartupCheckpoint();
}

// Does stop
void Module::stop() throw()
{
    Thread::stop();
    m_container.stop();
}

// Adds a message for processing
// @param[in] msg the pointer to the message that has to be added
void Module::addMessage(const IMessagePtr& msg)
{
    BOOST_ASSERT(msg);

    klk_log(KLKLOG_DEBUG, "Module '%s' got a message (id: %s; UUID: %d)",
            this->getName().c_str(),
            msg->getID().c_str(), msg->getUUID());

    if (msg->getType() == msg::SYNC_RES)
    {
        // sync response message
        Result rc = m_sync_container.add(msg);
        BOOST_ASSERT(rc == OK);
    }
    else
    {
        // all other messages
        Result rc = m_container.add(msg);
        if (!isStopped())
        {
            // should assert only for non stopped module
            BOOST_ASSERT(rc == OK);
        }
    }
}

// Sends a synchronous message (need for a response)
// for processing
// @see sendASyncMessage
// @param[in] in - the pointer to the message that has to be sent
// @param[out] out - the output message
void Module::sendSyncMessage(
    const IMessagePtr& in, IMessagePtr& out)
{
    // initial set
    in->setType(msg::SYNC_REQ);
    in->setSenderID(this->getID());

    // get receiver
    const StringList list = in->getReceiverList();
    BOOST_ASSERT(list.size() == 1);

    const std::string receiver_id = *(list.begin());
    IModulePtr
        receiver = getFactory()->getModuleFactory()->getModule(receiver_id);
    if (receiver == NULL)
    {
        throw Exception(__FILE__, __LINE__,
                        "Cannot get receiver module: %s. "
                        "Message id: %s. "
                        "Main module id: %s.",
                        receiver_id.c_str(), in->getID().c_str(),
                        getFactory()->getMainModuleId().c_str());
    }

    BOOST_ASSERT(receiver);

    klk_log(KLKLOG_DEBUG,
            "Send message (id: %s; UUID: %d) %s -> %s",
            in->getID().c_str(), in->getUUID(),
            this->getName().c_str(), receiver->getName().c_str());

    receiver->addMessage(in);

    BOOST_ASSERT(out == NULL);
    while (!out)
    {
        Result rc = m_sync_container.get(out);
        if (rc != OK)
        {
            // send trap
            m_factory->getSNMP()->sendTrap(snmp::MODULE_DNT_RESPONDE,
                                           receiver->getID());
            throw Exception(__FILE__, __LINE__,
                                 "Module %s could not get response for "
                                 "sync message with id "
                                 "'%s' (UUID: %d) within '%d' sec.",
                                 getName().c_str(),
                                 in->getID().c_str(), in->getUUID(),
                                 TIMEINTERVAL4SYNCRES);
        }

#if 0 //FIXME!!! dvbstreamer sends a lot of messages during unload
        if (isStopped())
        {
            klk_log(KLKLOG_ERROR,
                    "A sync message was not processed at module '%s'",
                    getName().c_str());
            KLKASSERT(0);
            return ERROR;
        }
#endif
        BOOST_ASSERT(out);

        if (out->getUUID() != in->getUUID())
        {
            klk_log(
                KLKLOG_ERROR,
                "The  module'%s' rejected message (id: %s, uuid: %d)",
                this->getName().c_str(), out->getID().c_str(),
                out->getUUID());
            out.reset();
        }
    }

    BOOST_ASSERT(out->getID() == in->getID());
    BOOST_ASSERT(out->getUUID() == in->getUUID());
}

// Gets info for CLI
const cli::ICommandList Module::getCLIInfo() const throw()
{
    return m_processor.getCLIInfo();
}

// Register a CLI command for processing
// @param[in] command - the command to be registred
void Module::registerCLI(const cli::ICommandPtr& command)

{
    BOOST_ASSERT(command);
    // add adapter dependency
    addDependency(adapter::MODID);
    // register command
    command->setFactory(m_factory);
    command->setModuleID(getID());
    m_processor.registerCLI(command);
}

// Register a sync message
void Module::registerASync(const std::string& msgid,
                         BaseFunction func)
{
    // add msgcore as dependency
    addDependency(msgcore::MODID);
    m_processor.registerASync(msgid, func);
}


// Register a sync message
void Module::registerSync(const std::string& msgid,
                            SyncFunction func)
{
    m_processor.registerSync(msgid, func);
}

// Adds a dependency on another module
void Module::addDependency(const std::string& modid)
{
    if (modid == getID())
        return; // nothing to do
    Result rc =
        m_factory->getModuleFactory()->addDependency(getID(), modid);
    if (rc != OK)
    {
        klk_log(KLKLOG_ERROR,
                "Setting dependency failed '%s' depends on '%s'",
                getID().c_str(), modid.c_str());
        BOOST_ASSERT(rc == OK);
    }
}

// Retrives version
const std::string Module::getVersion() const throw()
{
    return VERSION_SHORT_STR;
}

// Gets uptime inseconds
const time_t Module::getUptime() const
{
    return (time(NULL) - m_start_time);
}

// Retrive CPU usage info in the following interval
// (0, 1)
const double Module::getCPUUsage() const
{
    BOOST_ASSERT(m_usage);
    try
    {
        return m_usage->getCPUUsage();
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "Cannot retrive CPU usage info");
    }

    return -1;
}

// Register a thread
void Module::registerThread(const IThreadPtr& thread)
{
    m_scheduler.addThread(thread);
}

// Register a timer
void Module::registerTimer(TimerFunction f, const time_t intrv)
{
    try
    {
        IThreadPtr thread = IThreadPtr(new TimerThread(f, intrv));
        registerThread(thread);
    }
    catch(const std::bad_alloc&)
    {
        throw Exception(__FILE__, __LINE__, err::MEMORYALLOC);
    }
}

// Register an SNMP processor
void Module::registerSNMP(snmp::DataProcessor f,
                          const std::string& sockname)
{
    try
    {
        IThreadPtr thread = IThreadPtr(new snmp::Processor(f, sockname));
        registerThread(thread);
    }
    catch(const std::bad_alloc&)
    {
        throw Exception(__FILE__, __LINE__, err::MEMORYALLOC);
    }
}

/// @copydoc klk::IModule::waitStart
void  Module::waitStart(time_t timeout)
{
    if (!isStarted())
    {
        // wait untill we pass all checkpoints
        Result rc = m_start_sem.timedwait(timeout);
        if (rc != klk::OK)
        {
            Locker lock(&m_checkpoint_mutex);
            throw Exception(__FILE__, __LINE__,
                            "Timeout (%d) exceed while loading module %s (%s). "
                            "'%d' checkpoint left",
                            timeout, getName().c_str(),
                            getID().c_str(),
                            m_checkpoint_count);
        }
    }
}

// Register a new wait checkpoint
void Module::registerStartupCheckpoint()
{
    Locker lock(&m_checkpoint_mutex);
    m_checkpoint_count++;
    klk_log(KLKLOG_DEBUG, "Module '%s': "
            "'%d' checkpoints registered", getName().c_str(), m_checkpoint_count);

}

// Startup checkpoint pass event
void Module::passStartupCheckpoint()
{
    Locker lock(&m_checkpoint_mutex);
    m_checkpoint_count--;
    klk_log(KLKLOG_DEBUG, "Module '%s' startup is in progress."
            "'%d' checkpoints left", getName().c_str(), m_checkpoint_count);

    if (m_checkpoint_count <= 0)
    {
        m_checkpoint_count = 0;
        m_start_sem.post();
    }
}

/// @copydoc klk::IModule::isStarted
bool  Module::isStarted() const
{
    Locker lock(&m_checkpoint_mutex);
    if (m_checkpoint_count > 0)
    {
        klk_log(KLKLOG_DEBUG, "Module '%s' startup is in progress."
                "'%d' checkpoints left", getName().c_str(), m_checkpoint_count);
        return false;
    }
    return true;
}
