/**
   @file basemessage.cpp
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

#include "basemessage.h"
#include "exception.h"

using namespace klk;

//
// BaseMessage class
//

// Constructor
BaseMessage::BaseMessage(const std::string& id, const msg::UUID& uuid,
                         const std::string& sender_id) :
    m_lock(), m_id(id), m_uuid(uuid), m_sender_id(sender_id)
{
}

// Gets message type
msg::Type BaseMessage::getType() const
{
    Locker lock(&m_lock);
    BOOST_ASSERT(m_type != msg::UNDEF);
    return m_type;
}

// Sets message type
// @param[in] type - the type to be set
void BaseMessage::setType(msg::Type type)
{
    Locker lock(&m_lock);
    m_type = type;
}

// Gets sender module ID
// @return the ID
const std::string BaseMessage::getSenderID() const
{
    Locker lock(&m_lock);
    // it really can be empty if it's used for serialization
    // FIXME!!!
    //BOOST_ASSERT(!m_sender_id.empty());
    return m_sender_id;
}

// Sets sender module ID
// @param[in] id - the id to be set
void BaseMessage::setSenderID(const std::string& id)
{
    Locker lock(&m_lock);
    BOOST_ASSERT(!id.empty());
    m_sender_id = id;
}
