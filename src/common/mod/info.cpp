/**
   @file info.cpp
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
   - 2009/03/25 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "info.h"
#include "exception.h"

using namespace klk;
using namespace klk::mod;


//
// Info class
//

// Constructor
Info::Info(const std::string& uuid, const std::string& name) :
    m_lock(), m_uuid(uuid), m_name(name), m_inuse(false)
{
    if (m_uuid.empty())
    {
        throw Exception(__FILE__, __LINE__, "Invalid UUID for module info");
    }
    if (m_name.empty())
    {
        throw Exception(__FILE__, __LINE__, "Invalid name for module info");
    }
}

// Destructor
Info::~Info()
{
}

// Retrives the file uuid
const std::string Info::getUUID() const throw()
{
    Locker lock(&m_lock);
    return m_uuid;
}

// Retrives the file name
const std::string Info::getName() const throw()
{
    Locker lock(&m_lock);
    return m_name;
}

// Sets a name
void Info::setName(const std::string& name)
{
    BOOST_ASSERT(name.empty() == false);
    Locker lock(&m_lock);
    m_name = name;
}

// Tests if the info is used by somebody
bool Info::isInUse() throw()
{
    Locker lock(&m_lock);
    return m_inuse;
}

// Sets in use flag
void Info::setInUse(bool value)
{
    Locker lock(&m_lock);
    m_inuse = value;
}

// Updates the module info
void Info::updateInfo(const InfoPtr& value)
{
    BOOST_ASSERT(value);
    Locker lock(&m_lock);
    BOOST_ASSERT(value->m_name.empty() == false);
    m_name = value->m_name;
}

// Fills the output message with values from the info instance
void Info::fillOutMessage(const IMessagePtr& out) const
{
    BOOST_ASSERT(out);
    Locker lock(&m_lock);
    out->setData(msg::key::MODINFONAME, m_name);
}

// Checks that info corresponds data stored at the info message
void Info::check(const IMessagePtr& in) const
{
    // nothing to do by default
    // FIXME!!! add some default checks
}





