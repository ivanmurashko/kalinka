/**
   @file dvinfo.cpp
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
   - 2009/03/27 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/lexical_cast.hpp>

#include "dvinfo.h"
#include "messages.h"
#include "exception.h"

using namespace klk;
using namespace klk::fw;

//
// DVInfo class
//

// Constructor
DVInfo::DVInfo(const std::string& uuid, const std::string& name,
               const std::string& description) :
    mod::Info(uuid, name), m_description(description),
    m_port("-1")
{
    BOOST_ASSERT(m_description.empty() == false);
}

// Destructor
DVInfo::~DVInfo()
{
}

// Retrives the description
const std::string DVInfo::getDescription() const throw()
{
    Locker lock(&m_lock);
    return m_description;
}

// Sets port
void DVInfo::setPort(const int port)
{
    try
    {
        Locker lock(&m_lock);
        m_port = boost::lexical_cast<std::string>(port);
    }
    catch(const boost::bad_lexical_cast&)
    {
        throw Exception(__FILE__, __LINE__, err::BADLEXCAST);
    }
}

// Gets port
const std::string DVInfo::getPort() const throw()
{
    Locker lock(&m_lock);
    return m_port;
}

// Updates the module info
void DVInfo::updateInfo(const mod::InfoPtr& value)
{
    const DVInfoPtr dvinfo = boost::dynamic_pointer_cast<DVInfo>(value);
    BOOST_ASSERT(dvinfo);
    mod::Info::updateInfo(value);
    Locker lock(&m_lock);
    m_description = dvinfo->m_description;
}

// fills the message with info from the class instance
void DVInfo::fillOutMessage(const IMessagePtr& out) const
{
    Locker lock(&m_lock);
    mod::Info::fillOutMessage(out);
    out->setData(msg::key::IEEE1394PORT, m_port);
}

