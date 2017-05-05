/**
   @file testresources.cpp
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
   - 2009/01/25 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testresources.h"
#include "testfactory.h"
#include "netdev.h"
#include "db.h"
#include "log.h"
#include "exception.h"
#include "defines.h"
#include "testutils.h"
#include "testdefines.h"

using namespace klk;
using namespace klk::net;

//
// TestResources class (test resources)
//

// Constructor
void TestResources::setUp()
{
    test::Factory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);
    factory->reset();

    // setting up db connection
    IConfig *config = factory->getConfig();
    CPPUNIT_ASSERT(config != NULL);
    config->load();

    m_db = new db::DB(factory);
    CPPUNIT_ASSERT(m_db != NULL);
    m_db->connect();

    // clear DB before tests
    m_launcher = new test::DBScriptLauncher(config);
    m_launcher->recreate();

    // load module this one will initialized dvb resources
    factory->getModuleFactory()->load(MODID);
}

// Destructor
void TestResources::tearDown()
{
    test::Factory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);

    // unload modules
    factory->getModuleFactory()->unloadAll();

    factory->reset();

    m_launcher->recreate();
    KLKDELETE(m_launcher);
    KLKDELETE(m_db);
}

// Tests resources
void TestResources::test()
{
    test::printOut("\nNetwork resources management test ... ");

    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);
    IResourcesPtr
        resources =  factory->getModuleFactory()->getResources(MODID);
    CPPUNIT_ASSERT(resources);

    // test at the db (don't have lo by now)
    testLoopback(false);

    // update db info
    factory->getResources()->updateDB(dev::NET);

    // we should have a lo here
    testFullResources();
}

// Tests empty resources
void TestResources::testEmptyResources()
{
    // check that we don't have a lo network device here
    IDevList devs;
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);
    IDevPtr dev = factory->getResources()->getResourceByName(LOOPBACK_NAME);
    CPPUNIT_ASSERT(dev == NULL);
}

// Tests filled resources
void TestResources::testFullResources()
{
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);

    // now we should have not empty devs list
    IDevList devs = factory->getResources()->getResourceByType(dev::NET);
    CPPUNIT_ASSERT(devs.size() >= 1);

    // get by name test
    IDevPtr dev =
        factory->getResources()->getResourceByName(LOOPBACK_NAME);
    CPPUNIT_ASSERT(dev->getStringParam(dev::NETADDR) == "127.0.0.1");
    CPPUNIT_ASSERT(dev->getStringParam(dev::NETMASK) == "255.0.0.0");

    // test at the db
    testLoopback(true);

}

// Tests loopback interfaces
void TestResources::testLoopback(bool existence)
{
    //test_network_resources_list` (
    // klk_resources.resource_name,
    // klk_network_interface_addresses.ip_address,
    // klk_network_interface_addresses.ip_mask

    CPPUNIT_ASSERT(m_db != NULL);
    db::Parameters params;
    params.add("@host_uuid");
    db::ResultVector rv = m_db->callSelect("klk_network_resources_list",
                                           params, NULL);

    bool waslo = false;
    for (db::ResultVector::iterator i = rv.begin(); i != rv.end(); i++)
    {
        if ((*i)["resource_name"].toString() == LOOPBACK_NAME)
        {
            CPPUNIT_ASSERT(waslo == false);
            CPPUNIT_ASSERT((*i)["ip_address"].toString() == "127.0.0.1");
            CPPUNIT_ASSERT((*i)["ip_mask"].toString() == "255.0.0.0");
            waslo = true;
        }
    }
    if (existence)
    {
        CPPUNIT_ASSERT(waslo == true);
    }
    else
    {
        CPPUNIT_ASSERT(waslo == false);
    }
}

