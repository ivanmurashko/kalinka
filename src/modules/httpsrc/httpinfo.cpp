/**
   @file httpinfo.cpp
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
   - 2009/12/19 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "httpinfo.h"
#include "exception.h"
#include "messages.h"

using namespace klk;
using namespace klk::httpsrc;

//
// HTTPInfo class
//

// Constructor
HTTPInfo::HTTPInfo(const std::string& uuid,
                   const std::string& name,
                   const std::string& addr,
                   const std::string& login,
                   const std::string& passwd) :
    mod::Info(uuid, name), m_addr(addr),
    m_login(login), m_passwd(passwd)
{
}

// Fills output message
void HTTPInfo::fillOutMessage(const IMessagePtr& out) const
{
    BOOST_ASSERT(out);

    Locker lock(&m_lock);

    out->setData(msg::key::HTTPSRC_ADDR, m_addr);
    out->setData(msg::key::HTTPSRC_LOGIN, m_login);
    out->setData(msg::key::HTTPSRC_PWD, m_passwd);
}

// Updates the module info
void HTTPInfo::updateInfo(const mod::InfoPtr& value)
{
    /**
       The smart pointer for the info
    */
    typedef boost::shared_ptr<HTTPInfo> HTTPInfoPtr;

    const HTTPInfoPtr info = boost::dynamic_pointer_cast<HTTPInfo>(value);
    BOOST_ASSERT(info);
    Locker lock(&m_lock);
    mod::Info::updateInfo(value);
    m_addr = info->m_addr;
    m_login = info->m_login;
    m_passwd = info->m_passwd;
}
