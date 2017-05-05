/**
   @file testfactory.cpp
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
   - 2007/Jul/03 created by ipp
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include <boost/bind.hpp>

#include "common.h"
#include "maintest.h"
#include "testfactory.h"
#include "testmodfactory.h"
#include "log.h"
#include "paths.h"
#include "snmp/traptest.h"
#include "snmp/prototest.h"
#include "restest.h"
#include "conftest.h"
#include "dbtest.h"
#include "testid.h"
#include "xmltest.h"
#include "xml.h"
#include "utils.h"
#include "deptest.h"
#include "cliapptest.h"
#include "clitest.h"
#include "exception.h"
#include "socktest.h"
#include "modinfotest.h"

using namespace klk;
using namespace klk::test;

//
// Factory class
//

klk::test::Factory* klk::test::Factory::m_instance = 0;

// Constructor
Factory::Factory() :
    base::Factory("KLK-TEST"), m_ids(), m_handles()
{
}

// Gets an unique instance of the class
// "Replaces" constructor
Factory* Factory::instance()
{
    if (m_instance == NULL)
    {
	m_instance = new test::Factory();
        CPPUNIT_ASSERT(m_instance != NULL);
        m_instance->createAll();
    }
    return m_instance;
}

// Replaces the destructor
void Factory::destroy()
{
    KLKDELETE(m_instance);
}

// Destructor
Factory::~Factory()
{
    m_handles.clear();
    m_ids.clear();
}

// Gets the list of all modules in the form of std::string
std::string Factory::getTestIDs()
{
    CPPUNIT_ASSERT(!m_ids.empty());
    return m_ids;
}

// Creates all objects
void Factory::createAll()
{
    // set path for config
    CPPUNIT_ASSERT(getConfig() != NULL);
    getConfig()->setPath(dir::CFG_TEST);

    // load the configuration
    getConfig()->load();

    m_ids = ALL + ", ";

    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Main, MAIN);
    CPPUNIT_REGISTRY_ADD(MAIN, ALL);
    m_ids += MAIN + ", ";


    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(XMLTest, XML);
    CPPUNIT_REGISTRY_ADD(XML, ALL);
    m_ids += XML + ", ";

    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Config, CONFIG);
    CPPUNIT_REGISTRY_ADD(CONFIG, ALL);
    m_ids += CONFIG + ", ";

    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(db::DBTest, DB);
    CPPUNIT_REGISTRY_ADD(DB, ALL);
    m_ids += DB;

    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(snmp::TrapTest, SNMP);
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(snmp::ProtocolTest, SNMP);
    CPPUNIT_REGISTRY_ADD(SNMP, ALL);
    m_ids += SNMP + ", ";

    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(dev::ResourcesTest,
                                          RESOURCES);
    CPPUNIT_REGISTRY_ADD(RESOURCES, ALL);
    m_ids += RESOURCES + ", ";

    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(mod::DepTest, DEPS);
    CPPUNIT_REGISTRY_ADD(DEPS, ALL);
    m_ids += DEPS + ", ";

    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(CLIAppTest, CLIAPP);
    CPPUNIT_REGISTRY_ADD(CLIAPP, ALL);
    m_ids += CLIAPP  + ", ";

    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(cli::Test, CLI);
    CPPUNIT_REGISTRY_ADD(CLI, ALL);
    m_ids += CLI  + ", ";

    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(mod::InfoTest, MODINFO);
    CPPUNIT_REGISTRY_ADD(MODINFO, ALL);
    m_ids += MODINFO  + ", ";

    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(SocketTest, SOCKET);
    CPPUNIT_REGISTRY_ADD(SOCKET, ALL);
    m_ids += SOCKET;

    // inits module tests
    try
    {
        std::string folder = dir::SHARE + "/modules";
        StringList list = base::Utils::getFiles(folder);
        std::for_each(list.begin(), list.end(),
                      boost::bind(&Factory::initModUTest, this, _1));

        // and applications
        folder = dir::SHARE + "/applications";
        list = base::Utils::getFiles(folder);
        std::for_each(list.begin(), list.end(),
                      boost::bind(&Factory::initModUTest, this, _1));
    }
    catch(const std::exception&)
    {
        KLKASSERT(0);
    }

    klk_log(KLKLOG_DEBUG, "Unit tests data was created");
}


// Inits a module for testing
// @param[in] ifname - the file path
void Factory::initModUTest(const std::string& fname)
{
    try
    {
        // parse only files with .xml extension
        if (base::Utils::getFileExt(fname) != "xml")
        {
            klk_log(KLKLOG_DEBUG,
                    "Error file modules utest initialization from %s: "
                    "was ignored. Not XML file",
                    fname.c_str());
            return; // nothing to do
        }

        klk::XML xml;
        xml.parseFromFile(fname);
        std::string lib_path = xml.getValue("/klkdata/utest/lib_path");
        std::string testid = xml.getValue("/klkdata/utest/id");

        void *handle = getModUTestFun(lib_path);
        BOOST_ASSERT(handle);

        typedef void (*modfactory_func_type) ();
        modfactory_func_type fn =
            reinterpret_cast<modfactory_func_type>(handle);
        (*fn)();
        m_ids += ", " + testid;
    }
    catch (const std::exception& err)
    {
        klk_log(KLKLOG_ERROR,
                "Error file modules utest initialization from %s: %s",
                fname.c_str(), err.what());
        return;
    }
}

// Gets make function for unit test initialization
// @param[in] ifname - the lib path
// @return a result of dlsym or NULL if tehre was an error
void* Factory::getModUTestFun(const std::string& libpath)
{
    Locker lock(&m_lock);
    const std::string fname = "klk_module_test_init";
    LibContainerMap::iterator iter = m_handles.find(libpath);
    if (iter != m_handles.end())
    {
        KLKASSERT(iter->second);
        return iter->second->sym(fname);
    }

    // we should open the lib and add it
    LibContainerPtr lib(new LibContainer(libpath));
    lib->open();

    void *res = lib->sym(fname);
    BOOST_ASSERT(res);
    m_handles.insert(LibContainerMap::value_type(libpath, lib));
    return res;
}

// Reset prev state (especially the db
void Factory::reset()
{
    // clear known resources
    if (m_module_factory)
    {
        KLKASSERT(dynamic_cast<ModuleFactory*>(m_module_factory));
        dynamic_cast<ModuleFactory*>(m_module_factory)->clear();
    }
    KLKDELETE(m_resources);
}

// Gets's module factory interface
// @return the module's factory
IModuleFactory* Factory::getModuleFactory()
{
    Locker lock(&m_lock);
    if (m_module_factory == NULL)
    {
        m_module_factory = new test::ModuleFactory(this);
        KLKASSERT(m_module_factory != NULL);
    }
    return m_module_factory;
}
