/**
   @file msgfactory.cpp
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

#include <boost/bind.hpp>
#include <boost/assert.hpp>

#include "msgfactory.h"
#include "common.h"
#include "log.h"
#include "message.h"
#include "xml.h"
#include "exception.h"
#include "binarydata.h"

using namespace klk;

//
// MessageFactory class
//

// Constructor
MessageFactory::MessageFactory() :
    klk::Mutex(), m_uuid(0)
{
}

// Destructor
MessageFactory::~MessageFactory()
{
}

// Gets response message
// construct it from the input arg
// @param[in] request - the request message
// @return the response message
IMessagePtr MessageFactory::getResponse(const IMessagePtr& request) const

{
    BOOST_ASSERT(request);
    BOOST_ASSERT(request->getType() == msg::SYNC_REQ);
    IMessagePtr response(new Message(request->getID(),
                                     request->getUUID()));
    response->setType(msg::SYNC_RES);
    return response;
}


// Gets message by its id
// @param[in] id the message's id
IMessagePtr MessageFactory::getMessage(const std::string& id)
{
    klk::Locker lock(this);
    ASyncMessageInfoMap::iterator async = m_async_container.find(id);
    if (async != m_async_container.end())
    {
        IMessagePtr msg(new Message(id, generateUUID()));
        std::for_each(async->second.begin(), async->second.end(),
                      boost::bind(&IMessage::addReceiver, msg, _1));
        msg->setType(msg::ASYNC);
        return msg;
    }

    SyncMessageInfoMap::iterator sync = m_sync_container.find(id);
    if (sync == m_sync_container.end())
    {
        // not found
        throw klk::Exception(__FILE__, __LINE__,
                             "Message with id '%s' was not found",
                             id.c_str());
    }

    IMessagePtr msg(new Message(id, generateUUID()));
    msg->addReceiver(sync->second);
    msg->setType(msg::SYNC_REQ);
    return msg;
}

// Checks if the message register or not
bool MessageFactory::hasMessage(const std::string& id)
{
    klk::Locker lock(this);
    if (m_async_container.empty() == false)
    {
        ASyncMessageInfoMap::iterator i = m_async_container.find(id);
        if (i != m_async_container.end())
        {
            return true;
        }
    }
    if (m_sync_container.empty() == false)
    {
        SyncMessageInfoMap::iterator i = m_sync_container.find(id);
        if (i != m_sync_container.end())
        {
            return true;
        }
    }

    // not found
    return false;
}



// Gets a new unique UUID
msg::UUID MessageFactory::generateUUID()
{
    klk::Locker lock(this);
    return m_uuid++;
}

// Register a message
// @param[in] msg_id - the message's id to be registered
// @param[in] mod_id - the module's id that do the registration
// @param[in] msg_type - the message's type
void MessageFactory::registerMessage(const std::string& msg_id,
                                     const std::string& mod_id,
                                     msg::Type msg_type)

{
    klk::Locker lock(this);
    switch (msg_type)
    {
    case msg::SYNC_REQ:
    {
        if (m_sync_container.find(msg_id) != m_sync_container.end())
        {
            throw klk::Exception(
                __FILE__, __LINE__,
                "Trying to add sync message '%s' more than one times. "
                "Current mod_id: %s. Prev. mod id: %s.",
                msg_id.c_str(), mod_id.c_str(),
                m_sync_container[msg_id].c_str());
        }

        m_sync_container[msg_id] = mod_id;
        break;
    }
    case msg::ASYNC:
    {
        if (std::find(m_async_container[msg_id].begin(),
                      m_async_container[msg_id].end(), mod_id) !=
            m_async_container[msg_id].end())
        {
            throw klk::Exception(
                __FILE__, __LINE__,
                "Trying to add async message '%s' more than one times",
                msg_id.c_str());
        }
        m_async_container[msg_id].push_back(mod_id);
        break;
    }
    default:
        BOOST_ASSERT(false);
        break;
    }
}


// UnRegister a message
// @param[in] msg_id - the message's id to be unregistered
// @param[in] mod_id - the module's id that do the unregistration
// @param[in] msg_type - the message's type
void MessageFactory::unregisterMessage(const std::string& msg_id,
                                       const std::string& mod_id) throw()
{
    klk::Locker lock(this);
    SyncMessageInfoMap::iterator i = m_sync_container.find(msg_id);
    if (i != m_sync_container.end())
    {
        m_sync_container.erase(i);
    }
    else
    {
        ASyncMessageInfoMap::iterator i = m_async_container.find(msg_id);
        if (i != m_async_container.end())
        {
            StringList::iterator j = std::find(i->second.begin(),
                                               i->second.end(), mod_id);
            KLKASSERT(j != i->second.end());
            i->second.erase(j);
        }
        else
        {
            // some times we can be here (especially for messages registered by external apps)
            // just log
            klk_log(KLKLOG_ERROR, "The message '%s' registered by module '%s' "
                    "is already unregistered",
                    msg_id.c_str(), mod_id.c_str());
        }
    }
}
