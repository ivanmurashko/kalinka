/**
   @file testmodule.cpp
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
   - 2008/07/27 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testmodule.h"
#include "testfactory.h"
#include "testmodule.h"
#include "resources.h"
#include "log.h"

// modules specific info
#include "msgcore/defines.h"
#include "adapter/defines.h"

using namespace klk;
using namespace klk::test;

//
// TestModule class
//

// Constructor
TestModule::TestModule() :
    CppUnit::TestFixture(), m_modfactory(NULL),
    m_msgfactory(NULL), m_snmp_receiver()
{
}

// Destructor
TestModule::~TestModule()
{
}


// Constructor
void TestModule::setUp()
{
    CPPUNIT_ASSERT(m_modfactory == NULL);
    CPPUNIT_ASSERT(m_msgfactory == NULL);
    CPPUNIT_ASSERT(test::Factory::instance() != NULL);

    // reset test factory
    test::Factory::instance()->reset();

    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory != NULL);

    // getting module factory
    m_modfactory = dynamic_cast<test::ModuleFactory*>(
        factory->getModuleFactory());
    CPPUNIT_ASSERT(m_modfactory != NULL);

    // getting message factory
    m_msgfactory = factory->getMessageFactory();
    CPPUNIT_ASSERT(m_msgfactory);

    CPPUNIT_ASSERT(m_modfactory != NULL);
    CPPUNIT_ASSERT(m_msgfactory != NULL);

    // always load msgcore
    m_modfactory->load(msgcore::MODID);

    /*
      Resources* resources =
      dynamic_cast<Resources*>(factory->getResources());
      CPPUNIT_ASSERT(resources);

      resources->clear();
    */

    // start receiver
    m_snmp_receiver.start();

    loadModules();
}


// Destructor
void TestModule::tearDown()
{
    CPPUNIT_ASSERT(m_modfactory);
    // clearing
    m_modfactory->unloadAll();

    m_modfactory = NULL;
    m_msgfactory = NULL;

    // clear resources
    test::Factory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);
    factory->reset();
    factory->getResources()->initDevs();

    // stop SNMP receiver
    m_snmp_receiver.stop();
}


// Loads adapter module
void TestModule::loadAdapter()
{
    // load adapter
    m_modfactory->load(adapter::MODID);

    // sleep for awhile untill daemon will be really started
    sleep(3);
}

//
// TestModuleWithDB class
//

// Constructor
TestModuleWithDB::TestModuleWithDB(const std::string& sqlscript) :
    TestModule(), m_launcher(NULL),  m_db(NULL),
    m_sqlscript(sqlscript)
{
}

// Constructor
TestModuleWithDB::TestModuleWithDB() :
    TestModule(), m_launcher(NULL),  m_db(NULL),
    m_sqlscript()
{
}


// Destructor
TestModuleWithDB::~TestModuleWithDB()
{
    KLKDELETE(m_launcher);
    KLKDELETE(m_db);
}


// Constructor
void TestModuleWithDB::setUp()
{
    CPPUNIT_ASSERT(m_launcher == NULL);

    IFactory *factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory != NULL);

    CPPUNIT_ASSERT(factory->getConfig() != NULL);
    factory->getConfig()->load();

    m_launcher = new DBScriptLauncher(factory->getConfig());
    CPPUNIT_ASSERT(m_launcher != NULL);
    m_launcher->recreate();

    if (m_sqlscript.empty() == false)
    {
        m_launcher->executeScript(m_sqlscript);
    }

    m_db = new db::DB(factory);
    m_db->connect();

    TestModule::setUp() ;
}

// Destructor
void TestModuleWithDB::tearDown()
{
    //klk::Result rc = m_launcher->recreate();
    //CPPUNIT_ASSERT(rc == klk::OK);

    KLKDELETE(m_launcher);
    KLKDELETE(m_db);

    TestModule::tearDown();
}
