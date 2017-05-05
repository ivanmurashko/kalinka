/**
   @file testnetwork.cpp
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

#include "network.h"
#include "testnetwork.h"
#include "defines.h"
#include "testdefines.h"
#include "testid.h"
#include "testhelpmodule.h"
#include "testfactory.h"
#include "netdev.h"
#include "testresources.h"
#include "testcli.h"
#include "testrouteinfo.h"
#include "testutils.h"

using namespace klk;
using namespace klk::net;

// The function inits module's unit test
void klk_module_test_init()
{
    const std::string TESTRESOURCE = MODNAME + "/resource";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestResources, TESTRESOURCE);
    CPPUNIT_REGISTRY_ADD(TESTRESOURCE, MODNAME);

    const std::string TESTMAIN = MODNAME + "/main";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestNetwork, TESTMAIN);
    CPPUNIT_REGISTRY_ADD(TESTMAIN, MODNAME);

    const std::string TESTCLI = MODNAME + "/cli";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestCLI, TESTCLI);
    CPPUNIT_REGISTRY_ADD(TESTCLI, MODNAME);

    const std::string TESTROUTE = MODNAME + "/route";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestRouteInfo, TESTROUTE);
    CPPUNIT_REGISTRY_ADD(TESTROUTE, MODNAME);

    CPPUNIT_REGISTRY_ADD(MODNAME, test::ALL);
}

//
// TestNetwork class
//


// Constructor
TestNetwork::TestNetwork() :
    test::TestModuleWithDB("testnetwork.sql")
{
}

// Loads all necessary modules at setUp()
void TestNetwork::loadModules()
{
    CPPUNIT_ASSERT(m_modfactory);

    // create necessary dev
    createDev();

    // load the network module
    m_modfactory->load(MODID);

    // add helper module
    this->addDebug<TestHelpModule>();


    // load helper module
    m_modfactory->load(test::HELP_MOD_ID);
}


// Creates devs
void TestNetwork::createDev()
{
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);

    IResourcesPtr
        resources = factory->getModuleFactory()->getResources(MODID);
    CPPUNIT_ASSERT(resources);

    IDevPtr dev(new dev::Net(factory));
    dev->setParam(dev::NAME, TESTDEV_NAME);
    dev->setParam(dev::NETADDR, TESTDEV_ADDR);
    dev->setParam(dev::NETMASK, TESTDEV_MASK);
    resources->addDev(dev);
}

// The unit test itself
void TestNetwork::testMain()
{
    test::printOut("\nNetwork main test ... ");

    // give some time to module load
    sleep(3);

    // check that we have our test dev
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);
    IDevPtr dev = factory->getResources()->getResourceByName(TESTDEV_NAME);
    CPPUNIT_ASSERT(dev->getStringParam(dev::NETADDR) == TESTDEV_ADDR);

    // start the processing
    IMessagePtr start_msg = m_msgfactory->getMessage(test::HELP_MSG_ID);
    CPPUNIT_ASSERT(start_msg);
    m_modfactory->sendMessage(start_msg);

    sleep(200);

    Result rc = getModule<TestHelpModule>(test::HELP_MOD_ID)->getResult();
    CPPUNIT_ASSERT(rc == OK);
}

