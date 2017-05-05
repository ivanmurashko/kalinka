/**
   @file restest.cpp
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
   - 2008/09/13 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <map>
#include <memory>

#include "restest.h"
#include "testfactory.h"
#include "common.h"
#include "paths.h"
#include "dbscriptlauncher.h"
#include "busdev.h"
#include "exception.h"
#include "testutils.h"

using namespace klk;
using namespace klk::dev;

/**
   Test dev type
*/
const std::string TESTDEV1 = "test_dev1";

/**
   Test dev type
*/
const std::string TESTDEV2 = "test_dev2";

//
// class TestResources
//

// Constructor
TestResources::TestResources(const std::string& type) :
    BaseResources(test::Factory::instance(), type)
{
}


//
// TestDev class
//

// Constructor
// @param[in] factory - the factory
TestDev::TestDev(IFactory* factory) :
    dev::Base(factory, TESTDEV1)
{
}

// Updates the dev info
void TestDev::update()
{
    // nothing to do here
}

// Updates DB (adds the dev if it's missing)
void TestDev::updateDB()
{
    // nothing to do here
}

// test equalance of two devs
bool TestDev::equal_to(const IDevPtr& dev) const
{
    return getStringParam(NAME) == dev->getStringParam(NAME);
}

//
// ResourcesTest class
//

// Constructor
void ResourcesTest::setUp()
{
    // setting up db connection
    m_factory = test::Factory::instance();
    CPPUNIT_ASSERT(m_factory != NULL);

    IConfig *config = m_factory->getConfig();
    CPPUNIT_ASSERT(config != NULL);
    config->load();

    // clear DB before tests
    m_launcher = new test::DBScriptLauncher(config);
    m_launcher->recreate();
}

// Destructor
void ResourcesTest::tearDown()
{
    m_launcher->recreate();
    KLKDELETE(m_launcher);
}

/**
   Test dev type
*/
const std::string TESTRES = "test_res";


// Tests resources
void ResourcesTest::test()
{
    test::printOut("\nResources management test ... ");

    test::printOut("\n\tMain test ... ");
    CPPUNIT_ASSERT(m_factory != NULL);
    std::auto_ptr<BaseResources> resources(new Resources(m_factory));
    CPPUNIT_ASSERT(resources->m_devlist.empty());
    resources->initDevs();
    CPPUNIT_ASSERT(resources->m_devlist.size() == 1);
    IDevList list = resources->getResourceByType(dev::PCIBUS);
    CPPUNIT_ASSERT(list.size() == 1);
    IDevPtr pci = *list.begin();
    CPPUNIT_ASSERT(pci->getStringParam(dev::TYPE) == dev::PCIBUS);

    IDevPtr test_uuid =
        resources->getResourceByUUID(pci->getStringParam(dev::UUID));
    CPPUNIT_ASSERT(test_uuid->getStringParam(dev::TYPE) ==
                   dev::PCIBUS);

    // try to add a new PCIBus (should be failed
    IDevPtr pci_new(new dev::PCIBus(m_factory));
    resources->addDev(pci_new);
    CPPUNIT_ASSERT(resources->m_devlist.size() == 1);
    pci_new->update();
    CPPUNIT_ASSERT(resources->m_devlist.size() == 1);

    // test sub resources
    test::printOut("\n\tSub test ... ");
    // initial
    CPPUNIT_ASSERT(resources->hasDev(PCIBUS) == true);
    CPPUNIT_ASSERT(resources->hasDev(BASE) == true);
    CPPUNIT_ASSERT(resources->hasDev(TESTDEV1) == false);
    CPPUNIT_ASSERT(resources->hasDev(TESTDEV2) == false);
    CPPUNIT_ASSERT(resources->hasDev(TESTRES) == false);

    // create sub resources
    IResourcesPtr sub(new TestResources(TESTRES));
    CPPUNIT_ASSERT(sub != NULL);
    IDevPtr dev1 = IDevPtr(new TestDev(m_factory));
    CPPUNIT_ASSERT(dev1 != NULL);

    const std::string INTKEY = "test_int_key";

    const std::string uuid1 = "uuid1";
    dev1->setParam(dev::UUID, uuid1);
    const std::string name1 = "name1";
    dev1->setParam(dev::NAME, name1);
    dev1->setParam(INTKEY, 1);
    IDevPtr dev2 = IDevPtr(new TestDev(m_factory));
    CPPUNIT_ASSERT(dev2 != NULL);
    const std::string uuid2 = "uuid2";
    dev2->setParam(dev::UUID, uuid2);
    const std::string name2 = "name2";
    dev2->setParam(dev::NAME, name2);
    dev2->setParam(INTKEY, 2);
    IDevPtr dev3 = IDevPtr(new TestDev(m_factory));
    CPPUNIT_ASSERT(dev3 != NULL);
    const std::string uuid3 = "uuid3";
    dev3->setParam(dev::UUID, uuid3);
    dev3->setParam(dev::TYPE, TESTDEV2);
    const std::string name3 = "name3";
    dev3->setParam(dev::NAME, name3);
    dev3->setParam(INTKEY, 3);

    sub->addDev(dev1);
    // try to add several times (only first will be accepted)
    sub->addDev(dev1);
    sub->addDev(dev2);
    sub->addDev(dev3);
    resources->add(sub);

    // test presence
    CPPUNIT_ASSERT(resources->hasDev(TESTDEV1) == true);
    CPPUNIT_ASSERT(resources->hasDev(TESTDEV2) == true);
    CPPUNIT_ASSERT(resources->hasDev(TESTRES) == true);
    CPPUNIT_ASSERT(resources->hasDev(PCIBUS) == true);
    CPPUNIT_ASSERT(resources->hasDev(BASE) == true);

    // test find by name
    IDevPtr test_dev = resources->getResourceByName(PCIBUSNAME);
    CPPUNIT_ASSERT(test_dev->getStringParam(UUID) ==
                   pci->getStringParam(UUID));
    test_dev = resources->getResourceByName(name1);
    CPPUNIT_ASSERT(test_dev->getStringParam(dev::UUID) == uuid1);
    CPPUNIT_ASSERT(test_dev->getIntParam(INTKEY) == 1);

    test_dev = resources->getResourceByName(name2);
    CPPUNIT_ASSERT(test_dev->getStringParam(dev::UUID) == uuid2);
    CPPUNIT_ASSERT(test_dev->getIntParam(INTKEY) == 2);

    test_dev = resources->getResourceByName(name3);
    CPPUNIT_ASSERT(test_dev->getStringParam(dev::UUID) == uuid3);
    CPPUNIT_ASSERT(test_dev->getIntParam(INTKEY) == 3);


    // test the sub resources
    test_uuid = resources->getResourceByUUID(uuid1);
    CPPUNIT_ASSERT(test_uuid->getStringParam(dev::TYPE) == TESTDEV1);
    test_uuid = resources->getResourceByUUID(uuid2);
    CPPUNIT_ASSERT(test_uuid->getStringParam(dev::TYPE) == TESTDEV1);
    CPPUNIT_ASSERT(test_uuid->getIntParam(INTKEY) == 2);

    list = resources->getResourceByType(dev::PCIBUS);
    CPPUNIT_ASSERT(list.size() == 1);
    pci = *list.begin();
    CPPUNIT_ASSERT(pci->getStringParam(dev::TYPE) == dev::PCIBUS);

    // we have added 2 devices here
    // one of them 2 times
    // as result we should have 2 devices (not 3) here
    list = resources->getResourceByType(TESTDEV1);
    CPPUNIT_ASSERT(list.size() == 2);
    bool was1 = false, was2 = false;
    for (IDevList::iterator i = list.begin(); i != list.end(); i++)
    {
        if ((*i)->getStringParam(dev::UUID) == uuid1)
        {
            CPPUNIT_ASSERT(was1 == false);
            was1 = true;
            CPPUNIT_ASSERT((*i)->getStringParam(dev::TYPE) ==
                           TESTDEV1);
        }
        else if ((*i)->getStringParam(dev::UUID) == uuid2)
        {
            CPPUNIT_ASSERT(was2 == false);
            was2 = true;
            CPPUNIT_ASSERT((*i)->getStringParam(dev::TYPE) ==
                           TESTDEV1);
        }
        else
        {
            CPPUNIT_ASSERT(0);
        }
    }
    CPPUNIT_ASSERT(was1);
    CPPUNIT_ASSERT(was2);
    list = resources->getResourceByType(TESTDEV2);
    CPPUNIT_ASSERT(list.size() == 1);

    // getting whole tree
    list = resources->getResourceByType(TESTRES);
    CPPUNIT_ASSERT(list.size() == 3);
    bool was3 = false;
    was1 = false;
    was2 = false;
    for (IDevList::iterator i = list.begin(); i != list.end(); i++)
    {
        if ((*i)->getStringParam(dev::UUID) == uuid1)
        {
            CPPUNIT_ASSERT(was1 == false);
            was1 = true;
            CPPUNIT_ASSERT((*i)->getStringParam(dev::TYPE) ==
                           TESTDEV1);
        }
        else if ((*i)->getStringParam(dev::UUID) == uuid2)
        {
            CPPUNIT_ASSERT(was2 == false);
            was2 = true;
            CPPUNIT_ASSERT((*i)->getStringParam(dev::TYPE) ==
                           TESTDEV1);
        }
        else if ((*i)->getStringParam(dev::UUID) == uuid3)
        {
            CPPUNIT_ASSERT(was3 == false);
            was3 = true;
            CPPUNIT_ASSERT((*i)->getStringParam(dev::TYPE) ==
                           TESTDEV2);
        }
        else
        {
            CPPUNIT_ASSERT(0);
        }
    }
    CPPUNIT_ASSERT(was1);
    CPPUNIT_ASSERT(was2);
    CPPUNIT_ASSERT(was3);
}

