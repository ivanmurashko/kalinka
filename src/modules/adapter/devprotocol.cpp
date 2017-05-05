/**
   @file devprotocol.cpp
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

#include "devprotocol.h"
#include "defines.h"
#include "exception.h"

using namespace klk::adapter;

//
// DevProtocol class
//

// Constructor
DevProtocol::DevProtocol(IFactory* factory) :
    Protocol<ipc::IDevProxyPrx>(factory, obj::DEV, klk::MODULE_COMMON_ID)
{
}

// Retrives the dev state
klk::dev::State DevProtocol::getState(const std::string& uuid)
{
    return static_cast<klk::dev::State>(m_proxy->getState(uuid));
}


// Sets device state
void DevProtocol::setState(const std::string& uuid, klk::dev::State state)
{
    return m_proxy->setState(uuid, static_cast<int>(state));
}

// Tests is there a specified parameter available or not
bool DevProtocol::hasParam(const std::string& uuid, const std::string& key)
{
    return m_proxy->hasParam(uuid, key);
}

// Gets dev info by it's key
const std::string DevProtocol::getStringParam(const std::string& uuid,
                                              const std::string& key)
{
    return m_proxy->getStringParam(uuid, key);
}

// Gets dev info by it's key
int DevProtocol::getIntParam(const std::string& uuid,  const std::string& key)
{
    return m_proxy->getIntParam(uuid, key);
}

// Sets a value for a dev's parameter
void DevProtocol::setParam(const std::string& uuid,
                           const std::string& key,
                           const std::string& value,
                           bool mutable_flag)
{
    return m_proxy->setStringParam(uuid, key, value, mutable_flag);
}

// Sets a value for a dev's parameter
void DevProtocol::setParam(const std::string& uuid,
                           const std::string& key, int value,
                           bool mutable_flag)
{
    return m_proxy->setIntParam(uuid, key, value, mutable_flag);
}

// Gets last update time
time_t DevProtocol::getLastUpdateTime(const std::string& uuid)
{
    return m_proxy->getLastUpdateTime(uuid);
}
