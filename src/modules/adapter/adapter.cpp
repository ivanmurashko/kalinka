/**
   @file adapter.cpp
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
   - 2008/08/22 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sstream>
#include <iostream>

#include <boost/bind.hpp>

#include "adapter.h"
#include "common.h"
#include "log.h"
#include "defines.h"
#include "exception.h"
#include "db.h"
#include "messagesproxy.h"
#include "resourcesproxy.h"
#include "devproxy.h"
#include "modulesproxy.h"
#include "messagesprotocol.h"
#include "ipcmsg.h"

// modules specific info
#include "msgcore/defines.h"

using namespace klk;
using namespace klk::adapter;

/**
   Module factory function

   each module lib should define it
*/
IModulePtr klk_module_get(IFactory *factory)
{
    return IModulePtr(new Adapter(factory));
}

//
// Adapter
//

// Constructor
// @param[in] factory the module factory
Adapter::Adapter(IFactory *factory) :
    Module(factory, MODID), m_communicator(NULL),
    m_remote_messages(), m_remote_mutex()
{
    // add dependency on messaging core
    addDependency(msgcore::MODID);
}

// Destructor
Adapter::~Adapter()
{
    if (m_communicator)
    {
        postMainLoop();
    }
}

// Do init before startup
void Adapter::preMainLoop()
{
    Module::preMainLoop();

    try
    {
        // should not be initialized before
        if (m_communicator)
        {
            throw Exception(__FILE__, __LINE__,
                            "Try to initialize ICE more than 1 time");
        }
        int argc = 0;
        char* argv[] = {NULL};
        m_communicator = Ice::initialize(argc, argv);

        m_communicator->getProperties()->setProperty(
            "Ice.ThreadPool.Server.Size", "5");
        m_communicator->getProperties()->setProperty(
            "Ice.ThreadPool.Server.SizeMax", "10");

        // always export messages
        initObjectAdapter(obj::MESSAGES);

        if (getFactory()->getMainModuleId() == klk::MODULE_COMMON_ID)
        {
            // additional objects are exported by the main
            // application (mediaserver application)
            initObjectAdapter(obj::RESOURCES);
            initObjectAdapter(obj::DEV);
            initObjectAdapter(obj::MODULES);
        }
    }
    catch (const Ice::Exception& err)
    {
        throw Exception(__FILE__, __LINE__, "ICE exception: %s", err.what());
    }
    catch (const char* msg)
    {
        BOOST_ASSERT(msg);
        throw Exception(__FILE__, __LINE__, "ICE exception: %s", msg);
    }

    klk_log(KLKLOG_INFO, "Adapter module has been started");
}

// Do final cleanup after stop
void Adapter::postMainLoop() throw()
{
    if (m_communicator)
    {
        try
        {
            m_communicator->destroy();
        }
        catch (const Ice::Exception& err)
        {
            klk_log(KLKLOG_ERROR, "Error while ICE cleanup: %s", err.what());
        }
        m_communicator = NULL;
    }

    try
    {
        std::for_each(m_adapters.begin(), m_adapters.end(),
                      boost::bind(&Adapter::updateDB4ObjectAdapter, this,
                                  _1, std::string()));
        m_adapters.clear();
    }
    catch(const std::exception& err)
    {
        klk_log(KLKLOG_ERROR,
                "Exception gotten in the adapter module post loop: %s",
                err.what());
    }
    catch(...)
    {
        klk_log(KLKLOG_ERROR,
                "Exception gotten in the adapter module post loop: unknown");
    }

    Module::postMainLoop();
}

// Gets a human readable module name
// @return the name
const std::string Adapter::getName() const throw()
{
    return MODNAME;
}

// Register all processors
void Adapter::registerProcessors()
{
    // nothing to do for the module
}

// Inits the specified object adapter
void Adapter::initObjectAdapter(const std::string& object_id)
{
    BOOST_ASSERT(m_communicator);

    Ice::ObjectAdapterPtr adapter
        = m_communicator->createObjectAdapterWithEndpoints(
            object_id, ICE_CONNECTION_STR);
    Ice::ObjectPtr object = createObject(object_id);
    Ice::Identity identity = m_communicator->stringToIdentity(object_id);
    adapter->add(object, identity);
    adapter->activate();
    const std::string connection_string =
        adapter->createProxy(identity)->ice_toString();
    updateDB4ObjectAdapter(object_id, connection_string);

    // FIXME!!! not thread safe (is the reentrancy is really necessary???)
    m_adapters.push_back(object_id);

    klk_log(KLKLOG_INFO,
            "Adapter module startup: ICE endpoint for '%s': %s",
            object_id.c_str(), connection_string.c_str());
}

// Updates DB settings for object adapter specified at the argument
void Adapter::updateDB4ObjectAdapter(const std::string& object_id,
                                     const std::string& connection_string)
{
    // `klk_adapter_update` (
    // IN host_uuid VARCHAR(40),
    // IN module_id VARCHAR(40),
    // IN object_id VARCHAR(40),
    // IN endpoint VARCHAR(255),
    // OUT return_value INT
    // retrive route information
    db::DB db(getFactory());
    db.connect();

    db::Parameters params;
    params.add("@host", db.getHostUUID());
    params.add("@module", getFactory()->getMainModuleId());
    params.add("@object", object_id);
    if (connection_string.empty())
    {
        params.add("@endpoint");
    }
    else
    {
        params.add("@endpoint", connection_string);
    }
    params.add("@return_value");

    db::Result res = db.callSimple("klk_adapter_update", params);
    if (res["@return_value"].toInt() != 0)
    {
        throw Exception(__FILE__, __LINE__,
                        "DB error while adapter setting update");
    }

}

// Creates ICE object ptr by object identifier
const Ice::ObjectPtr Adapter::createObject(const std::string& object_id)
{
    Ice::ObjectPtr result;
    // FIXME!!! bad code
    if (object_id == obj::MESSAGES)
        result = new ipc::MessagesProxy(getFactory(), this);
    else if (object_id == obj::RESOURCES)
        result = new ipc::ResourcesProxy(getFactory());
    else if (object_id == obj::DEV)
        result = new ipc::DevProxy(getFactory());
    else if (object_id == obj::MODULES)
        result = new ipc::ModulesProxy(getFactory());
    else
        throw klk::Exception(__FILE__, __LINE__, "Unknown object id: %s",
                             object_id.c_str());
    return result;
}

// Register a remote message for processing
void Adapter::registerRemoteMessage(const std::string& msg_id,
                                    const std::string& mod_id,
                                    msg::Type msg_type)
{
    {
        klk::Locker lock(&m_remote_mutex);
        // check that the module id has not been registered yet
        StringList ids = m_remote_messages[msg_id];
        if (std::find(ids.begin(), ids.end(), mod_id) == ids.end())
        {
            m_remote_messages[msg_id].push_back(mod_id);
        }
        else
        {
            // has been registered already
            klk_log(KLKLOG_ERROR, "The remote message '%s' for module '%s' "
                    "has been already registered at adapter module",
                    msg_id.c_str(), mod_id.c_str());
            return; // nothing to do
        }
    }

    switch (msg_type)
    {
    case msg::ASYNC:
        registerASync(msg_id, boost::bind(&Adapter::processRemoteASync, this, _1));
        break;
    case msg::SYNC_REQ:
        registerSync(msg_id, boost::bind(&Adapter::processRemoteSync, this, _1, _2));
        break;
    default:
        BOOST_ASSERT(false);
        break;
    }
}


// Processor for remote sync message
void Adapter::processRemoteSync(const IMessagePtr& in, const IMessagePtr& out)
{
    StringList receivers = getReceiverList(in);
    BOOST_ASSERT(receivers.empty() == false);
    for (StringList::iterator receiver = receivers.begin(); receiver != receivers.end();
        receiver++)
    {
        try
        {
            // update the receiver list
            in->clearReceiverList();
            in->addReceiver(*receiver);
            MessagesProtocol proto(getFactory(), *receiver);

            // FIXME!!! very bad code
            IMessagePtr res = proto.sendSync(in);
            ipc::SMessage msg_data =
                boost::dynamic_pointer_cast<ipc::Message>(res)->getICEData();
            // fill the data
            for (ipc::SimpleDataStorage::iterator item = msg_data.mValues.begin();
                 item != msg_data.mValues.end(); item++)
            {
                out->setData(item->first, item->second);
            }

            for (ipc::ListDataStorage::iterator item = msg_data.mLists.begin();
                 item != msg_data.mLists.end(); item++)
            {
                StringList list;
                std::copy( item->second.begin(),  item->second.end(),
                  std::back_inserter(list));
                out->setData(item->first, list);
            }
        }
        catch(const std::exception& err)
        {
            klk_log(KLKLOG_ERROR,
                    "Error while sending sync message with id (%s) to module with id (%s): %s",
                    in->getID().c_str(), receiver->c_str(), err.what());
        }
    }
}

// Processor for remote async message
void Adapter::processRemoteASync(const IMessagePtr& in)
{
    StringList receivers = getReceiverList(in);
    BOOST_ASSERT(receivers.empty() == false);
    for (StringList::iterator receiver = receivers.begin(); receiver != receivers.end();
        receiver++)
    {
        try
        {
            // update the receiver list
            in->clearReceiverList();
            in->addReceiver(*receiver);
            MessagesProtocol proto(getFactory(), *receiver);
            proto.sendASync(in);
        }
        catch(const std::exception& err)
        {
            klk_log(KLKLOG_ERROR,
                    "Error while sending async message with id (%s) to module with id (%s): %s",
                    in->getID().c_str(), receiver->c_str(), err.what());
        }
    }
}

// Retrieves remote receivers list (list of module ids) by the input message
const StringList Adapter::getReceiverList(const IMessagePtr& in)
{
    klk::Locker lock(&m_remote_mutex);
    return m_remote_messages[in->getID()];
}
