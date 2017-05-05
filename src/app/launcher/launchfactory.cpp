/**
   @file launchfactory.cpp
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
   - 2010/07/18 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/assert.hpp>

#include "launchfactory.h"
#include "launchresources.h"
#include "launchmodfactory.h"
#include "launchmsgfactory.h"

using namespace klk;
using namespace klk::launcher;

//
// Factory class
//

klk::launcher::Factory* klk::launcher::Factory::m_instance = 0;

// Constructor
Factory::Factory(const std::string& appname, const std::string& module_id) :
    base::Factory(appname.c_str()), m_module_id(module_id),
    m_launcher(this, module_id)
{
}

// Destructor
Factory::~Factory()
{
}

// @return the factory instance
Factory* Factory::instance(const std::string& appname,
                           const std::string& module_id)
{
    if (m_instance == NULL)
    {
        m_instance = new Factory(appname, module_id);
    }
    return m_instance;
}

// @return the factory instance
Factory* Factory::instance()
{
    BOOST_ASSERT(m_instance);
    return m_instance;
}

/// Frees allocated resources
void Factory::destroy()
{
    KLKDELETE(m_instance);
}

// @copydoc klk::IFactory::getResources
IResources* Factory::getResources()
{
    Locker lock(&m_lock);
    if (m_resources == NULL)
    {
        // we should load configuration before
        // to access our settings stored at the db
        getConfig()->load();
        m_resources = new Resources(this);
    }
    return m_resources;
}

// Gets's module factory interface
// @return the module's factory
IModuleFactory* Factory::getModuleFactory()
{
    Locker lock(&m_lock);
    if (m_module_factory == NULL)
    {
        m_module_factory = new ModuleFactory(this);
    }
    return m_module_factory;
}

// Gets's message factory interface
// @return the message's factory
IMessageFactory* Factory::getMessageFactory()
{
    Locker lock(&m_lock);
    if (m_message_factory == NULL)
    {
        m_message_factory = new MessageFactory(this);
    }
    return m_message_factory;
}
