/**
   @file converter.cpp
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

#include "converter.h"
#include "exception.h"

#include "ipcmsg.h"

#include "message.h"

namespace klk
{
    //
    // Message class specification
    //
    template <> const adapter::ipc::SMessage
    Message::convertTo(adapter::ipc::SMessage*) const
    {
        adapter::ipc::SMessage msg;
        msg.mID = getID();
        msg.mSender = getSenderID();
        msg.mValues = m_values;
        std::copy(m_receivers.begin(), m_receivers.end(),
                  std::back_inserter(msg.mReceivers));
        for (StringListMap::const_iterator value = m_lists.begin();
             value != m_lists.end(); value++)
        {
            std::copy(value->second.begin(), value->second.end(),
                  std::back_inserter(msg.mLists[value->first]));
        }
        return msg;
    }
}


using namespace klk;
using namespace klk::adapter;

//
// Converter class
//

// Constructor
Converter::Converter(IFactory* factory) : m_factory(factory)
{
    BOOST_ASSERT(m_factory);
}

// Ice -> klk::IMessage converter
const IMessagePtr Converter::ice2msg(const ipc::SMessage& msg) const
{
    IMessagePtr res(new ipc::Message(
                        msg,
                        m_factory->getMessageFactory()->generateUUID()));
    return res;
}

// klk::IMessage -> Ice converter
const ipc::SMessage Converter::msg2ice(const IMessagePtr& msg) const
{
    BOOST_ASSERT(msg);
    // FIXME!!! remove dynamic_cast
    // see ticket #183

    {
        boost::shared_ptr<Message> msg_cast =
            boost::dynamic_pointer_cast<Message>(msg);
        if (msg_cast)
        {
            return msg_cast->convertTo<ipc::SMessage>();
        }
    }

    // already prepared
    boost::shared_ptr<ipc::Message> msg_cast =
        boost::dynamic_pointer_cast<ipc::Message>(msg);
    BOOST_ASSERT(msg_cast);
    return msg_cast->getICEData();
}
