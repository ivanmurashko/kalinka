/**
   @file devproxy.cpp
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

#include "devproxy.h"

using namespace klk::adapter::ipc;

//
// DevProxy class
//

// Constructor
DevProxy::DevProxy(IFactory* factory) : m_resources(factory->getResources())
{
    BOOST_ASSERT(m_resources);
}


// Retrives the dev by it's uuid
const klk::IDevPtr DevProxy::getDev(const std::string& uuid)
{
    return m_resources->getResourceByUUID(uuid);
}


// Retrives the dev state
int DevProxy::getState(const std::string& uuid,  const Ice::Current&)
{
    return static_cast<int>(getDev(uuid)->getState());
}

// Sets device state
void DevProxy::setState(const std::string& uuid, int state, const Ice::Current&)
{
    getDev(uuid)->setState(static_cast<klk::dev::State>(state));
}

// Tests is there a specified parameter available or not
bool DevProxy::hasParam(const std::string& uuid, const std::string& key,
                        const Ice::Current&)
{
    return getDev(uuid)->hasParam(key);
}

// Gets dev info by it's key
std::string DevProxy::getStringParam(const std::string& uuid,
                                     const std::string& key, const Ice::Current&)
{
    return getDev(uuid)->getStringParam(key);
}

// Gets dev info by it's key
int DevProxy::getIntParam(const std::string& uuid,
                          const std::string& key, const Ice::Current&)
{
    return getDev(uuid)->getIntParam(key);
}

// Sets a value for a dev's parameter
void DevProxy::setStringParam(const std::string& uuid,
                              const std::string& key,
                              const std::string& value,
                              bool mutable_flag, const Ice::Current&)
{
    return getDev(uuid)->setParam(key, value, mutable_flag);
}

// Sets a value for a dev's parameter
void DevProxy::setIntParam(const std::string& uuid,
                           const std::string& key, int value,
                           bool mutable_flag, const Ice::Current&)
{
    return getDev(uuid)->setParam(key, value, mutable_flag);
}

// Gets last update time
int DevProxy::getLastUpdateTime(const std::string& uuid, const Ice::Current&)
{
    return getDev(uuid)->getLastUpdateTime();
}
