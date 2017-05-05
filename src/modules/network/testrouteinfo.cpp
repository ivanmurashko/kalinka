/**
   @file testrouteinfo.cpp
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
   - 2009/07/02 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testrouteinfo.h"
#include "testfactory.h"
#include "testdefines.h"
#include "routeinfo.h"
#include "defines.h"
#include "netdev.h"
#include "dbscriptlauncher.h"
#include "messages.h"
#include "testutils.h"

using namespace klk;
using namespace klk::net;

//
// TestRouteInfo class
//

// Constructor
TestRouteInfo::TestRouteInfo() : m_dev()
{
}

// Fills initial data for utest
void TestRouteInfo::setUp()
{
    test::Factory::instance()->reset();

    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);
    factory->getConfig()->load();

    // fills DB
    test::DBScriptLauncher launcher(factory->getConfig());
    launcher.recreate();
    launcher.executeScript("testnetwork.sql");

    // create the test dev
    createDev();
}

// Clears utest data
void TestRouteInfo::tearDown()
{
    test::Factory::instance()->reset();
    m_dev.reset();
}

// The unit test for route info
void TestRouteInfo::test()
{
    test::printOut("\nNetwork route info test ... ");

    // check that we have our test dev
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);
    CPPUNIT_ASSERT(m_dev);

    RouteInfo info1(TESTROUTE1_UUID, TESTROUTE1_NAME,
                    TESTROUTE1_ADDR, TESTROUTE1_PORT,
                    TCPIP,
                    msg::key::NETUNICAST,
                    m_dev, factory);

    test::printOut("\n\tBase ... ");

    // should not be locked
    CPPUNIT_ASSERT(info1.isInUse() == false);

    // lock it
    info1.setInUse(true);
    CPPUNIT_ASSERT(info1.isInUse() == true);

    // unlock
    info1.setInUse(false);
    CPPUNIT_ASSERT(info1.isInUse() == false);

    test::printOut("\n\tAuto unlock ... ");

    // automatic unlock
    info1.setInUse(true);
    CPPUNIT_ASSERT(info1.isInUse() == true);
    sleep(2*LOCK_UPDATE_INTERVAL + 2);
    CPPUNIT_ASSERT(info1.isInUse() == false);

    // no automatic unlock
    info1.setInUse(true);
    CPPUNIT_ASSERT(info1.isInUse() == true);
    sleep(LOCK_UPDATE_INTERVAL);
    info1.updateInUse();
    CPPUNIT_ASSERT(info1.isInUse() == true);

    sleep(LOCK_UPDATE_INTERVAL);
    info1.updateInUse();
    CPPUNIT_ASSERT(info1.isInUse() == true);

    sleep(5);
    CPPUNIT_ASSERT(info1.isInUse() == true);

    test::printOut("\n\tClearing ... ");

    // final clearing
    info1.clearInUse();
    CPPUNIT_ASSERT(info1.isInUse() == false);
}

// Creates devs
void TestRouteInfo::createDev()
{
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);

    IResourcesPtr
        resources = factory->getModuleFactory()->getResources(MODID);
    CPPUNIT_ASSERT(resources);

    CPPUNIT_ASSERT(m_dev == NULL);
    m_dev = IDevPtr(new dev::Net(factory));
    m_dev->setParam(dev::NAME, TESTDEV_NAME);
    m_dev->setParam(dev::NETADDR, TESTDEV_ADDR);
    m_dev->setParam(dev::NETMASK, TESTDEV_MASK);
    resources->addDev(m_dev);
}
