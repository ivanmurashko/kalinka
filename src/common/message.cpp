/**
   @file message.cpp
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
   - 2008/09/21 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sstream>

#include <boost/bind.hpp>

#include "message.h"
#include "common.h"
#include "log.h"
#include "utils.h"
#include "exception.h"

using namespace klk;

// Constructor
Message::Message(const std::string& id, msg::UUID uuid) :
    BaseMessage(id, uuid),
    m_values(), m_lists(), m_receivers()
{
}

// Adds a new receiver to the list
// @param[in] id - the receiver's id
void Message::addReceiver(const std::string& id)
{
    Locker lock(&m_lock);
    // check that it does not exist
    StringList::iterator i =
        std::find(m_receivers.begin(), m_receivers.end(), id);
    BOOST_ASSERT(i == m_receivers.end());
    m_receivers.push_back(id);
}

// Gets the list with modules IDs that should get the message
const StringList Message::getReceiverList() const throw()
{
    Locker lock(&m_lock);
    return m_receivers;
}

// Gets message specific data
// @param[in] key - the key name (data id)
const std::string Message::getValue(const std::string& key) const
{
    Locker lock(&m_lock);
    StringMap::const_iterator i = m_values.find(key);

    if (i == m_values.end())
    {
        throw klk::Exception(__FILE__, __LINE__,
                             "Cannot find simple data for key '%s' "
                             "in message with id '%s'",
                             key.c_str(), getID().c_str());
    }

    return i->second;
}

// Gets message specific data
// @param[in] key - the key name (data id)
const StringList Message::getList(const std::string& key) const
{
    Locker lock(&m_lock);
    StringListMap::const_iterator i = m_lists.find(key);

    if (i == m_lists.end())
    {
        throw klk::Exception(__FILE__, __LINE__,
                             "Cannot find list data for key '%s' "
                             "in message with id '%s'",
                             key.c_str(), getID().c_str());
    }

    return i->second;
}


// Sets message specific data
// @param[in] data - the data to be set
// @param[in] key - the key name (data id)
void Message::setData(const std::string& key, const std::string& data)
{
    Locker lock(&m_lock);
    m_values[key] = data;
}

// Sets message specific data
// @param[in] data - the data to be set
// @param[in] key - the key name (data id)
void Message::setData(const std::string& key, const StringList& data)
{
    Locker lock(&m_lock);
    m_lists[key] = data;
}

// Checks message specific data
bool Message::hasValue(const std::string& key) const
{
    Locker lock(&m_lock);
    return (m_values.find(key) != m_values.end());
}

#if 0
// Checks message specific data
bool Message::hasList(const std::string& key) const
{
    Locker lock(&m_lock);
    return (m_lists.find(key) != m_lists.end());
}
#endif

// Clears receiver list
void Message::clearReceiverList() throw()
{
    Locker lock(&m_lock);
    m_receivers.clear();
}

