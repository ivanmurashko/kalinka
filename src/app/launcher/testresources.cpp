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
   - 2010/08/29 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testresources.h"
#include "testdefines.h"
#include "restest.h"
#include "testfactory.h"
#include "launchfactory.h"
#include "testutils.h"

using namespace klk;
using namespace klk::launcher;

//
// TestResources class
//

namespace
{
    // First device name
    const std::string DEV1_NAME = "name1";
    // First device uuid
    const std::string DEV1_UUID = "uuid1";
    // First dev type
    const std::string DEV1_TYPE = "uuid1";

    // Second device name
    const std::string DEV2_NAME = "name2";
    // Second device uuid
    const std::string DEV2_UUID = "uuid2";

    // Third device name
    const std::string DEV3_NAME = "name3";
    // Third device uuid
    const std::string DEV3_UUID = "uuid3";

    // Sub devices group uuid
    const std::string SUBDEV_UUID = "subdev_uuid";

    // Type for dev1, dev2
    const std::string DEVTYPE1 = "type1";

    // Type for dev3
    const std::string DEVTYPE2 = "type2";
}

/// Constructor
TestResources::TestResources() : TestLauncher()
{
}


/// Destructor
TestResources::~TestResources()
{
}

/// @copydoc klk::test::TestModule::setUp()
void TestResources::setUp()
{
    // the parent class initialization
    TestLauncher::setUp();

    /**
       There are the following device map in the main factory:

       MAIN --
             |
             -- SUBDEV_UUID --
             |               |
             -- DEV1_UUID    -- DEV3_UUID
             |
             -- DEV2_UUID
     */

    IFactory* main_factory = test::Factory::instance();

    // sub resources
    IResourcesPtr sub(new dev::TestResources(SUBDEV_UUID));
    main_factory->getResources()->add(sub);

    // add resources
    IDevPtr dev1(new dev::TestDev(main_factory));
    dev1->setParam(dev::UUID, DEV1_UUID);
    dev1->setParam(dev::NAME, DEV1_NAME);
    dev1->setParam(dev::TYPE, DEVTYPE1);
    main_factory->getResources()->addDev(dev1);
    main_factory->getResources()->getResourceByUUID(DEV1_UUID);

    IDevPtr dev2(new dev::TestDev(main_factory));
    dev2->setParam(dev::UUID, DEV2_UUID);
    dev2->setParam(dev::NAME, DEV2_NAME);
    dev2->setParam(dev::TYPE, DEVTYPE1);
    main_factory->getResources()->addDev(dev2);


    IDevPtr dev3(new dev::TestDev(main_factory));
    dev3->setParam(dev::UUID, DEV3_UUID);
    dev3->setParam(dev::NAME, DEV3_NAME);
    dev3->setParam(dev::TYPE, DEVTYPE2);
    sub->addDev(dev3);
}



/// The unit test itself
void TestResources::test()
{
    test::printOut("\nLauncher resources test ... ");
    // all resources are present on remote factory
    IResources* remote_res = test::Factory::instance()->getResources();
    CPPUNIT_ASSERT(remote_res->hasDev(DEVTYPE1) == true);
    CPPUNIT_ASSERT(remote_res->hasDev(DEVTYPE2) == true);

    // resources are present at local factory
    IResources* local_res = Factory::instance()->getResources();
    CPPUNIT_ASSERT(local_res->hasDev(DEVTYPE1) == true);
    CPPUNIT_ASSERT(local_res->hasDev(DEVTYPE2) == true);

    // test all devices
    testDev(DEV1_UUID);
    testDev(DEV2_UUID);
    testDev(DEV3_UUID);
}

// Tests a device
void TestResources::testDev(const std::string& dev_uuid)
{
    // should be present on both local and remote
    IResources* remote_res = test::Factory::instance()->getResources();
    IDevPtr remote_dev = remote_res->getResourceByUUID(dev_uuid);
    CPPUNIT_ASSERT(remote_dev);
    IResources* local_res = Factory::instance()->getResources();
    IDevPtr local_dev = local_res->getResourceByUUID(dev_uuid);
    CPPUNIT_ASSERT(local_dev);

    // should have the same name
    CPPUNIT_ASSERT(local_dev->hasParam(dev::NAME) == true);
    CPPUNIT_ASSERT(remote_dev->hasParam(dev::NAME) == true);
    CPPUNIT_ASSERT(remote_dev->getStringParam(dev::NAME) ==
                   local_dev->getStringParam(dev::NAME));
    // by the name we should get the same device
    IDevPtr test_dev = local_res->getResourceByName(
        local_dev->getStringParam(dev::NAME));
    CPPUNIT_ASSERT(test_dev);
    CPPUNIT_ASSERT(test_dev->getStringParam(dev::UUID) == dev_uuid);
    test_dev = remote_res->getResourceByName(
        remote_dev->getStringParam(dev::NAME));
    CPPUNIT_ASSERT(test_dev);
    CPPUNIT_ASSERT(test_dev->getStringParam(dev::UUID) == dev_uuid);

    // test state
    CPPUNIT_ASSERT(local_dev->getState() == remote_dev->getState());
    local_dev->setState(dev::IDLE);
    CPPUNIT_ASSERT(local_dev->getState() == dev::IDLE);
    CPPUNIT_ASSERT(local_dev->getState() == remote_dev->getState());
    remote_dev->setState(dev::WORK);
    CPPUNIT_ASSERT(local_dev->getState() == dev::WORK);
    CPPUNIT_ASSERT(local_dev->getState() == remote_dev->getState());

    // test a parameter set
    const std::string key1 = "testkey1";
    CPPUNIT_ASSERT(local_dev->hasParam(key1) == false);
    CPPUNIT_ASSERT(remote_dev->hasParam(key1) == false);
    const std::string value1 = "testvalue1";
    local_dev->setParam(key1, value1);
    CPPUNIT_ASSERT(local_dev->hasParam(key1) == true);
    CPPUNIT_ASSERT(remote_dev->hasParam(key1) == true);
    CPPUNIT_ASSERT(remote_dev->getStringParam(key1) == value1);
    CPPUNIT_ASSERT(local_dev->getStringParam(key1) ==
                   remote_dev->getStringParam(key1));

    const std::string key2 = "testkey2";
    CPPUNIT_ASSERT(local_dev->hasParam(key2) == false);
    CPPUNIT_ASSERT(remote_dev->hasParam(key2) == false);
    const int value2 = 2;
    remote_dev->setParam(key2, value2);
    CPPUNIT_ASSERT(local_dev->hasParam(key2) == true);
    CPPUNIT_ASSERT(remote_dev->hasParam(key2) == true);
    CPPUNIT_ASSERT(remote_dev->getStringParam(key2) == "2");
    CPPUNIT_ASSERT(local_dev->getStringParam(key2) ==
                   remote_dev->getStringParam(key2));
    CPPUNIT_ASSERT(local_dev->getIntParam(key2) == value2);
    CPPUNIT_ASSERT(local_dev->getIntParam(key2) ==
                   remote_dev->getIntParam(key2));

}

