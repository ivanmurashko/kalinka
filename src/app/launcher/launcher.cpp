/**
   @file launcher.cpp
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
   - 2010/07/11 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/assert.hpp>

#include "launcher.h"
#include "log.h"
#include "exception.h"

#include "adapter/messagesprotocol.h"
#include "adapter/modulesprotocol.h"
#include "adapter/resourcesprotocol.h"
#include "adapter/devprotocol.h"

using namespace klk;
using namespace klk::launcher;

//
// Launcher class
//

// Constructor
Launcher::Launcher(IFactory* factory, const std::string& module_id) :
    m_factory(factory), m_module_id(module_id)
{
    BOOST_ASSERT(m_module_id.empty() == false);
}

// starts the application
void Launcher::start()
{
    // first of all we should check that main mediaserver application
    // has been started and is able to receive RPC
    checkRPC();

    // load the application module
    m_factory->getModuleFactory()->load(m_module_id);
    klk_log(KLKLOG_DEBUG, "Application with modid '%s' has been loaded",
            m_module_id.c_str());
    // wait until stop request
    m_factory->getEventTrigger()->wait();

    // stop the modules
    // FIXME!!! code dublicate with src/server/server.cpp
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

/// stops the launcher
void Launcher::stop()
{
    klk_log(KLKLOG_INFO, "Launcher get stop signal");
    // safe to call it from signal handlers
    m_factory->getEventTrigger()->stop();
}

// The methods checks that RPC is available. It checks connections
// to all necessary adapters: messages, modules, resources and dev
void Launcher::checkRPC()
{
    using namespace klk::adapter;
    try
    {
        ResourcesProtocol res_proto(m_factory);
        if (res_proto.checkConnection() != klk::OK)
        {
            throw klk::Exception(__FILE__, __LINE__,
                                 "Resources RPC is not available");
        }

        DevProtocol dev_proto(m_factory);
        if (dev_proto.checkConnection() != klk::OK)
        {
            throw klk::Exception(__FILE__, __LINE__,
                                 "Dev RPC is not available");
        }

        MessagesProtocol msg_proto(m_factory);
        if (msg_proto.checkConnection() != klk::OK)
        {
            throw klk::Exception(__FILE__, __LINE__,
                                 "Messages RPC is not available");
        }

        ModulesProtocol mod_proto(m_factory);
        if (mod_proto.checkConnection() != klk::OK)
        {
            throw klk::Exception(__FILE__, __LINE__,
                                 "Modules RPC is not available");
        }
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR, "RPC check failed");
        throw;
    }
}
