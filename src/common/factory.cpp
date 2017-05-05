/**
   @file factory.cpp
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
   - 2008/09/27 created by ipp (Ivan Murashko)
   - 2009/01/04 some exceptions were added by ipp
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <mysql.h>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#include <gst/gst.h>

#include "factory.h"
#include "klkconfig.h"
#include "resources.h"
#include "modfactory.h"
#include "msgfactory.h"
#include "snmp/snmp.h"

#include "common.h"
#include "log.h"
#include "paths.h"
#include "exception.h"
#include "common.h"

using namespace klk;
using namespace klk::base;

//
// Factory class
//

// Constructor
// @param[in] ident - The ident argument is a string
// that  is  prepended to  every log message.
Factory::Factory(const char* ident) :
    m_lock(),
    m_module_factory(NULL), m_message_factory(NULL),
    m_resources(NULL), m_config(NULL),
    m_snmp(NULL), m_stop()
{
    // open log
    klk_open_log(ident);

    // init xerces
    try
    {
        XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();
    }
    catch (const XERCES_CPP_NAMESPACE::XMLException& to_catch)
    {
        char* msg =
            XERCES_CPP_NAMESPACE::XMLString::transcode(to_catch.getMessage());
        KLKASSERT(msg != NULL);
        klk_log(KLKLOG_ERROR, "Lib xerces initialization error: %s", msg);
        XERCES_CPP_NAMESPACE::XMLString::release(&msg);
    }

    // init mysql
    mysql_library_init(-1, NULL, NULL);

    // check thread safety
    KLKASSERT(mysql_thread_safe() == 1);

    // init gstreamer
#if 1
    gst_init (NULL, NULL);
#else
    int argc = 2;
    char** argv = (char**)calloc(argc+1, sizeof(char*));
    argv[0] = (char*)"test";
    argv[1] = (char*)"--gst-debug-level=3";
    gst_init(&argc, &argv);
#endif
}

// Destructor
Factory::~Factory()
{
    KLKDELETE(m_resources);
    KLKDELETE(m_snmp);
    KLKDELETE(m_module_factory); // module factory first
    KLKDELETE(m_message_factory);
    KLKDELETE(m_config);

    KLKASSERT(m_config == NULL);
    KLKASSERT(m_resources == NULL);
    KLKASSERT(m_module_factory == NULL);
    KLKASSERT(m_message_factory == NULL);

    // terminate xerces
    XERCES_CPP_NAMESPACE::XMLPlatformUtils::Terminate();
    // terminate mysql
    mysql_library_end();

    klk_log(KLKLOG_DEBUG, "Factory main data was finitialized");

    // terminate log
    klk_close_log();
}


// Gets an interfaces to config info
// @return the interface
IConfig* Factory::getConfig()
{
    Locker lock(&m_lock);
    try
    {
        if (m_config == NULL)
        {
            m_config = new Config();
        }
    }
    catch(const std::bad_alloc&)
    {
        throw Exception(__FILE__, __LINE__, err::MEMORYALLOC);
    }
    BOOST_ASSERT(m_config);
    return m_config;
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

// Gets's module factory interface
// @return the module's factory
const IModuleFactory* Factory::getModuleFactory() const
{
    Locker lock(&m_lock);
    BOOST_ASSERT(m_module_factory);
    return m_module_factory;
}


// Gets's module factory interface
// @return the module's factory
IMessageFactory* Factory::getMessageFactory()
{
    Locker lock(&m_lock);
    if (m_message_factory == NULL)
    {
        m_message_factory = new MessageFactory();
    }
    BOOST_ASSERT(m_message_factory);
    return m_message_factory;
}

// Gets resources manager
IResources* Factory::getResources()
{
    Locker lock(&m_lock);
    if (m_resources == NULL)
    {
        dev::Resources* resources = NULL;
        // load config
        getConfig()->load();
        resources = new dev::Resources(this);
        resources->initDevs();
        m_resources = resources;
    }
    return m_resources;
}

// Gets resources manager
const IResources* Factory::getResources() const
{
    Locker lock(&m_lock);
    BOOST_ASSERT(m_resources);
    return m_resources;
}

// Gets SNMP interface
// @return the interface
ISNMP* Factory::getSNMP()
{
    Locker lock(&m_lock);
    if (m_snmp == NULL)
    {
        // load config
        getConfig()->load();
        // create db connector
        m_snmp = new snmp::SNMP(getConfig());
    }
    BOOST_ASSERT(m_snmp);
    return m_snmp;
}

/// @copydoc klk::IFactory::getMainModuleId()
const std::string Factory::getMainModuleId() const
{
    return MODULE_COMMON_ID;
}

