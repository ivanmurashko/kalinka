/**
   @file basedev.cpp
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
   - 2009/01/23 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "basedev.h"
#include "log.h"
#include "exception.h"

using namespace klk;
using namespace klk::dev;

//
// Base class
//

// Constructor
// @param[in] type - the device type
Base::Base(IFactory* factory, const std::string& type) :
    m_factory(factory), m_lock(),
    m_params(), m_last_update_time(time(NULL)),
    m_state(dev::IDLE)
{
    BOOST_ASSERT(type.empty() == false);
    m_params.insert(ParamContainer::value_type(dev::TYPE, type));
}

// tests a param availability
bool Base::hasParam(const std::string& key) const
{
    Locker lock(&m_lock);
    return (m_params.find(key) != m_params.end());
}


// Gets the factory
IFactory* Base::getFactory()
{
    BOOST_ASSERT(m_factory);
    return m_factory;
}

// Gets the factory
const IFactory* Base::getFactory() const
{
    BOOST_ASSERT(m_factory);
    return m_factory;
}

// Retrives variant parameter
const StringWrapper Base::getParam(const std::string& key) const
{
    Locker lock(&m_lock);
    ParamContainer::const_iterator i = m_params.find(key);
    if (i == m_params.end())
    {
        throw Exception(__FILE__, __LINE__,
                             "Unknown parameter '" + key + "' for device");
    }

    return i->second;
}

// Gets dev info by it's key in the form of a std::string
const std::string Base::getStringParam(const std::string& key) const
{
    return getParam(key).toString();
}

// Gets dev info by it's key in the form of an integer
const int Base::getIntParam(const std::string& key) const
{
    return getParam(key).toInt();
}

// Sets a parameter
template <class T> void Base::set(const std::string& key, const T& value,
                                  bool mutable_flag)
{
    Locker lock(&m_lock);
    ParamContainer::iterator find = m_params.find(key);
    if (find != m_params.end())
    {
        find->second = value;
    }
    else
    {
        m_params.insert(ParamContainer::value_type(key, value));
    }

    if (!mutable_flag)
    {
        m_last_update_time = time(NULL);
    }
}


// Sets a string value for a dev's parameter
void Base::setParam(const std::string& key,
                    const std::string& value,
                    bool mutable_flag)
{
    set<std::string>(key, value, mutable_flag);
}

// Sets an integer value for a dev's parameter
void Base::setParam(const std::string& key,
                    const int value,
                    bool mutable_flag)
{
    set<int>(key, value, mutable_flag);
}



// Gets last update time
const time_t Base::getLastUpdateTime() const
{
    Locker lock(&m_lock);
    return m_last_update_time;
}

// Retrives the dev state
dev::State Base::getState() const
{
    Locker lock(&m_lock);
    return m_state;
}

// Sets device state
void Base::setState(dev::State state)
{
    Locker lock(&m_lock);
    m_state = state;
}
