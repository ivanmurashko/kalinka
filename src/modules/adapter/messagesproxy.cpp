/**
   @file messagesproxy.cpp
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
   - 2010/04/24 created by ipp (Ivan Murashko)
   - 2010/09/12 class name was changed: Proxy -> MessagesProxy by ipp
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "messagesproxy.h"
#include "exception.h"

using namespace klk;
using namespace klk::adapter;
using namespace klk::adapter::ipc;

//
// MessagesProxy class
//

// Constructor
MessagesProxy::MessagesProxy(IFactory* factory, Adapter* module) :
    m_factory(factory), m_module(module),
    m_converter(factory)
{
    BOOST_ASSERT(m_factory);
    BOOST_ASSERT(m_module);
}

// Sends an async message
void MessagesProxy::sendASync(const SMessage& msg, const Ice::Current& crn)
{
    IMessagePtr input_msg = m_converter.ice2msg(msg);
    input_msg->setType(msg::ASYNC);
    m_factory->getModuleFactory()->sendMessage(input_msg);
}

// Sends a sync message
SMessage MessagesProxy::sendSync(const SMessage& msg, const Ice::Current& crn)
{
    IMessagePtr input_msg = m_converter.ice2msg(msg), output_msg;
    input_msg->setType(msg::SYNC_REQ);
    m_module->sendSyncMessage(input_msg, output_msg);

    return m_converter.msg2ice(output_msg);
}

// Creates a message by its id
SMessage MessagesProxy::getMessage(const std::string& id,
                                   const Ice::Current& crn)
{
    IMessagePtr msg = m_factory->getMessageFactory()->getMessage(id);
    return m_converter.msg2ice(msg);
}

// Register a message
void MessagesProxy::registerMessage(const std::string& msg_id,
                                    const std::string& mod_id, int msg_type,
                                    const Ice::Current& crn)
{
    m_module->registerRemoteMessage(msg_id, mod_id,
                                    static_cast<msg::Type>(msg_type));
}
