/**
   @file test1394.cpp
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
   - 2009/03/18 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "ieee1394.h"
#include "test1394.h"
#include "defines.h"
#include "testdefines.h"
#include "testid.h"
#include "testfactory.h"
#include "testhelpmodule.h"
#include "testutils.h"

using namespace klk;
using namespace klk::fw;

// The function inits module's unit test
void klk_module_test_init()
{
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestFireWire,
                                          MODNAME);
    CPPUNIT_REGISTRY_ADD(MODNAME, test::ALL);
}

//
// TestFireWire class
//


// Constructor
TestFireWire::TestFireWire() :
    test::TestModuleWithDB("testieee1394.sql")
{
}

// Loads all necessary modules at setUp()
void TestFireWire::loadModules()
{
    CPPUNIT_ASSERT(m_modfactory);
    // now load the it
    m_modfactory->load(MODID);

    // add helper module
    this->addDebug<TestHelpModule>();
    m_modfactory->load(TEST_MOD_ID);

}


// The unit test itself
void TestFireWire::test()
{
    test::printOut("\nIEEE 1394 test ... ");

    testInitial();

    testDetection();

    testRemove();
}

// Initial test
// DB keeps one record but list is empty
void TestFireWire::testInitial()
{
    test::printOut("\n\tInitial test ...");
    boost::shared_ptr<TestHelpModule> test_module =
        getModule<TestHelpModule>(TEST_MOD_ID);
    CPPUNIT_ASSERT(test_module);
    test_module->clear();
    boost::shared_ptr<FireWire> module = getModule<FireWire>(MODID);
    CPPUNIT_ASSERT(module);
    module->m_update =
        boost::bind(&TestFireWire::getRemoveDVInfoSet);
    sleep(CHECKINTERVAL + 2);
    // list is empty
    DVInfoList list = module->m_info.getInfoList();
    CPPUNIT_ASSERT(list.empty() == true);

    // check detection state
    CPPUNIT_ASSERT(test_module->getAppearCount1() == 0);
    CPPUNIT_ASSERT(test_module->getDisappearCount1() == 0);
    CPPUNIT_ASSERT(test_module->getAppearCount2() == 0);
    CPPUNIT_ASSERT(test_module->getDisappearCount2() == 0);


    // db keeps one record
    db::DB db(test::Factory::instance());
    db.connect();

    db::Parameters params;
    db::ResultVector rv = db.callSelect("klk_ieee1394_list", params, NULL);
    CPPUNIT_ASSERT(rv.size() == 1);
    db::Result res = *rv.begin();
    // SELECT uuid, name, description FROM klk_ieee1394;
    CPPUNIT_ASSERT(res["uuid"].toString() == TESTUUID1);
    CPPUNIT_ASSERT(res["name"].toString() == TESTNAME1);
    CPPUNIT_ASSERT(res["description"].toString() == TESTDESCR1);
}

// Detection test
// DB keeps two records and list has the records too
void TestFireWire::testDetection()
{
    test::printOut("\n\tDetection test ...");

    boost::shared_ptr<TestHelpModule> test_module =
        getModule<TestHelpModule>(TEST_MOD_ID);
    CPPUNIT_ASSERT(test_module);
    test_module->clear();
    boost::shared_ptr<FireWire> module = getModule<FireWire>(MODID);
    CPPUNIT_ASSERT(module);
    module->m_update =
        boost::bind(&TestFireWire::getFullDVInfoSet);
    sleep(CHECKINTERVAL + 2);
    // list not empty
    DVInfoList list = module->m_info.getInfoList();
    CPPUNIT_ASSERT(list.size() == 2);
    DVInfoList::iterator find1 = std::find_if(
        list.begin(), list.end(),
        boost::bind(mod::FindInfoByUUID<>(), _1, TESTUUID1));
    CPPUNIT_ASSERT(find1 != list.end());
    CPPUNIT_ASSERT((*find1)->getUUID() == TESTUUID1);
    CPPUNIT_ASSERT((*find1)->getName() == TESTNAME1);
    CPPUNIT_ASSERT((*find1)->getDescription() == TESTDESCR1);
    DVInfoList::iterator find2 = std::find_if(
        list.begin(), list.end(),
        boost::bind(mod::FindInfoByUUID<>(), _1, TESTUUID2));
    CPPUNIT_ASSERT(find2 != list.end());
    CPPUNIT_ASSERT((*find2)->getUUID() == TESTUUID2);
    CPPUNIT_ASSERT((*find2)->getName() == TESTNAME2);
    CPPUNIT_ASSERT((*find2)->getDescription() == TESTDESCR2);

    // check detection state
    CPPUNIT_ASSERT(test_module->getAppearCount1() == 1);
    CPPUNIT_ASSERT(test_module->getDisappearCount1() == 0);
    CPPUNIT_ASSERT(test_module->getAppearCount2() == 1);
    CPPUNIT_ASSERT(test_module->getDisappearCount2() == 0);

    // test records at the DB
    testFullDB();
}

// Remove test
// DB keeps two records but list is empty
void TestFireWire::testRemove()
{
    test::printOut("\n\tRemove test ...");

    boost::shared_ptr<TestHelpModule> test_module =
        getModule<TestHelpModule>(TEST_MOD_ID);
    CPPUNIT_ASSERT(test_module);
    test_module->clear();

    boost::shared_ptr<FireWire> module = getModule<FireWire>(MODID);
    CPPUNIT_ASSERT(module);
    module->m_update =
        boost::bind(&TestFireWire::getRemoveDVInfoSet);
    sleep(CHECKINTERVAL + 2);
    // list is empty
    DVInfoList list = module->m_info.getInfoList();
    CPPUNIT_ASSERT(list.empty() == true);

    // check detection state
    CPPUNIT_ASSERT(test_module->getAppearCount1() == 0);
    CPPUNIT_ASSERT(test_module->getDisappearCount1() == 1);
    CPPUNIT_ASSERT(test_module->getAppearCount2() == 0);
    CPPUNIT_ASSERT(test_module->getDisappearCount2() == 1);

    // disappear should not change because
    // no any existent devs were disapeared
    sleep(CHECKINTERVAL + 2);
    CPPUNIT_ASSERT(test_module->getAppearCount1() == 0);
    CPPUNIT_ASSERT(test_module->getDisappearCount1() == 1);
    CPPUNIT_ASSERT(test_module->getAppearCount2() == 0);
    CPPUNIT_ASSERT(test_module->getDisappearCount2() == 1);

    testFullDB();
}

// Tests full db
void TestFireWire::testFullDB()
{
    // db keeps 2 records
    db::DB db(test::Factory::instance());
    db.connect();
    db::Parameters params;
    db::ResultVector rv = db.callSelect("klk_ieee1394_list", params, NULL);
    CPPUNIT_ASSERT(rv.size() == 2);
    bool was1 = false, was2 = false;
    for (db::ResultVector::iterator i = rv.begin(); i != rv.end(); i++)
    {
        db::Result res = *i;
        if (res["uuid"].toString() == TESTUUID1)
        {
            CPPUNIT_ASSERT(was1 == false);
            was1 = true;
            CPPUNIT_ASSERT(res["name"].toString() == TESTNAME1);
            CPPUNIT_ASSERT(res["description"].toString() == TESTDESCR1);
        }
        else if (res["uuid"].toString() == TESTUUID2)
        {
            CPPUNIT_ASSERT(was2 == false);
            was2 = true;
            CPPUNIT_ASSERT(res["name"].toString() == TESTNAME2);
            CPPUNIT_ASSERT(res["description"].toString() == TESTDESCR2);
        }
        else
        {
            CPPUNIT_ASSERT(0);
        }
    }

    CPPUNIT_ASSERT(was1);
    CPPUNIT_ASSERT(was2);
}



// Returns an empty list with cameras
const InfoSet TestFireWire::getRemoveDVInfoSet()
{
    InfoSet result;
    return result;
}

// Returns a filled list with cameras
const InfoSet TestFireWire::getFullDVInfoSet()
{
    InfoSet set;
    set.insert(DVInfoPtr(new DVInfo(TESTUUID1, "blabla1", "test descr1")));
    set.insert(DVInfoPtr(new DVInfo(TESTUUID2, TESTNAME2, TESTDESCR2)));
    return set;
}
