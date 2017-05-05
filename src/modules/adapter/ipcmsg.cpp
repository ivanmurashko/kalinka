/**
   @file ipcmsg.cpp
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
   - 2010/05/02 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ipcmsg.h"
#include "exception.h"

using namespace klk;
using namespace klk::adapter;
using namespace klk::adapter::ipc;

//
// Message class
//

// Constructor
Message::Message(const SMessage& ice_data, const msg::UUID uuid) :
    BaseMessage(ice_data.mID, uuid, ice_data.mSender),
    m_ice_data(ice_data)
{
}

/// @copydoc klk::IMessage::getReceiverList()
const klk::StringList Message::getReceiverList() const throw()
{
    klk::StringList res;
    Locker lock(&m_lock);
    std::copy(m_ice_data.mReceivers.begin(), m_ice_data.mReceivers.end(),
              std::back_inserter(res));
    return res;
}

/// @copydoc klk::IMessage::addReceiver()
void Message::addReceiver(const std::string& id)
{
    Locker lock(&m_lock);
    // check only one presence of the id at the lis
    if (std::find(m_ice_data.mReceivers.begin(),
                  m_ice_data.mReceivers.end(), id) !=
        m_ice_data.mReceivers.end())
    {
        throw klk::Exception(__FILE__, __LINE__,
                             "Reciever with id '" + id + "'already exists");
    }

    m_ice_data.mReceivers.push_back(id);
}

/// @copydoc klk::IMessage::clearReceiverList()
void Message::clearReceiverList() throw()
{
    Locker lock(&m_lock);
    m_ice_data.mReceivers.clear();
}

/// @copydoc klk::IMessage::getReceiverList()
bool Message::hasValue(const std::string& key) const
{
    Locker lock(&m_lock);
    return (m_ice_data.mValues.find(key) != m_ice_data.mValues.end());
}

/// @copydoc klk::IMessage::getValue()
const std::string Message::getValue(const std::string& key) const
{
    Locker lock(&m_lock);
    SimpleDataStorage::const_iterator find = m_ice_data.mValues.find(key);

    if (find == m_ice_data.mValues.end())
    {
        throw klk::Exception(__FILE__, __LINE__,
                             "Cannot find simple data for key '%s' "
                             "in message with id '%s'",
                             key.c_str(), getID().c_str());
    }

    return find->second;
}

/// @copydoc klk::IMessage::getList()
const klk::StringList Message::getList(const std::string& key) const
{
    Locker lock(&m_lock);
    ListDataStorage::const_iterator find = m_ice_data.mLists.find(key);

    if (find == m_ice_data.mLists.end())
    {
        throw klk::Exception(__FILE__, __LINE__,
                             "Cannot find list data for key '%s' "
                             "in message with id '%s'",
                             key.c_str(), getID().c_str());
    }

    klk::StringList res;
    std::copy(find->second.begin(), find->second.end(),
              std::back_inserter(res));
    return res;
}

/// @copydoc klk::IMessage::setData()
void Message::setData(const std::string& key, const std::string& data)
{
    Locker lock(&m_lock);
    m_ice_data.mValues[key] = data;
}

/// @copydoc klk::IMessage::setData()
void Message::setData(const std::string& key, const klk::StringList& data)
{
    Locker lock(&m_lock);
    m_ice_data.mLists[key].clear();
    std::copy(data.begin(), data.end(),
              std::back_inserter(m_ice_data.mLists[key]));
}
