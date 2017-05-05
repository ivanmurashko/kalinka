/**
   @file messagesprotocol.cpp
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
   - 2010/09/19 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "messagesprotocol.h"
#include "defines.h"
#include "exception.h"


using namespace klk;
using namespace klk::adapter;

// Constructor
MessagesProtocol::MessagesProtocol(IFactory* factory, const std::string& main_module_id) :
    Protocol<ipc::IMessagesProxyPrx>(factory, obj::MESSAGES, main_module_id),
    m_converter(factory)
{
}

// Sends a message via the protocol
void MessagesProtocol::sendASync(const IMessagePtr& msg)
{
    m_proxy->sendASync(m_converter.msg2ice(msg));
}

// Sends a sync message via the protocol
// @param[in] msg - the message to be sent
const IMessagePtr MessagesProtocol::sendSync(const IMessagePtr& msg)
{
    ipc::SMessage input_ice = m_converter.msg2ice(msg);
    ipc::SMessage output_ice = m_proxy->sendSync(input_ice);
    return m_converter.ice2msg(output_ice);
}

// Creates a message by id
IMessagePtr MessagesProtocol::getMessage(const std::string& id)
{
    return m_converter.ice2msg(m_proxy->getMessage(id));
}

// Register a message
void MessagesProtocol::registerMessage(const std::string& msg_id,
                                       const std::string& mod_id,
                                       msg::Type msg_type)
{
    m_proxy->registerMessage(msg_id, mod_id, msg_type);
}
