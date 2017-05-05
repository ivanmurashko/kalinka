/**
   @file testdvb.cpp
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
   - 2008/10/12 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testdvb.h"
#include "log.h"
#include "testid.h"
#include "defines.h"
#include "testhelpmodule.h"
#include "testfactory.h"
#include "resources.h"
#include "dvb.h"
#include "paths.h"
#include "dvbdev.h"

#include "testresources.h"
#include "testdefines.h"
#include "testsnmp.h"
#include "testcli.h"
#include "messages.h"
#include "testutils.h"

using namespace klk;
using namespace klk::dvb;

// The function inits module's unit test
void klk_module_test_init()
{
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestResources, MODNAME);
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestDVB, MODNAME);
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestSNMP, MODNAME);
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestCLI, MODNAME);
    CPPUNIT_REGISTRY_ADD(MODNAME, test::ALL);
}

//
// TestDVB class
//


// Constructor
TestDVB::TestDVB() :
    TestBase()
{
}

// Loads all necessary modules at setUp()
void TestDVB::loadModules()
{
    CPPUNIT_ASSERT(m_modfactory);

    // add helper modules
    this->addDebug<TestHelpModule>();

    // load module
    m_modfactory->load(MODID);
    // load helper module
    m_modfactory->load(TEST_MOD_ID);

    // create devices
    createDevs();
}

// The unit test itself
void TestDVB::testStream()
{
    test::printOut("\nDVB stream test ... ");

    // start the processing
    IMessagePtr start_msg = m_msgfactory->getMessage(TEST_STREAM_MSG_ID);
    CPPUNIT_ASSERT(start_msg);

    // let check thread to be started
    sleep(10);

    m_modfactory->sendMessage(start_msg);

    sleep(5);

    klk::Result rc = getModule<TestHelpModule>(TEST_MOD_ID)->getResult();
    CPPUNIT_ASSERT(rc == klk::OK);

    // test the adapters get
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);
    const IDevList list =
        factory->getResources()->getResourceByType(klk::dev::DVB_ALL);
    CPPUNIT_ASSERT(list.size() == 3);

    // test info at DVB module

    // test devices
    testDevs();


    // we should have 2 channels on dev1 and one on dev2 and nothing
    // on dev3
    IDevPtr dev1 = factory->getResources()->getResourceByName(TESTCARD1);
    CPPUNIT_ASSERT(dev1);
    StringList l1 =
        getModule<DVB>(MODID)->m_processor->getChannelNames(dev1);
    CPPUNIT_ASSERT(l1.size() == 2);
    CPPUNIT_ASSERT(std::find(l1.begin(), l1.end(),
                             TEST_CHANNEL11_NAME) != l1.end());
    CPPUNIT_ASSERT(std::find(l1.begin(), l1.end(),
                             TEST_CHANNEL12_NAME) != l1.end());
    CPPUNIT_ASSERT(std::find(l1.begin(), l1.end(),
                             TEST_CHANNEL2_NAME) == l1.end());

    IDevPtr dev2 = factory->getResources()->getResourceByName(TESTCARD2);
    CPPUNIT_ASSERT(dev2);
    StringList l2 =
        getModule<DVB>(MODID)->m_processor->getChannelNames(dev2);
    CPPUNIT_ASSERT(l2.size() == 1);
    CPPUNIT_ASSERT(std::find(l2.begin(), l2.end(),
                             TEST_CHANNEL2_NAME) != l2.end());

    IDevPtr dev3 = factory->getResources()->getResourceByName(TESTCARD3);
    CPPUNIT_ASSERT(dev3);
    StringList l3 =
        getModule<DVB>(MODID)->m_processor->getChannelNames(dev3);
    CPPUNIT_ASSERT(l3.empty());
}

// Tests devs for streaming
void TestDVB::testDevs()
{
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);

    // test devices
    IDevPtr dev1 = factory->getResources()->getResourceByName(TESTCARD1);
    CPPUNIT_ASSERT(dev1);
    IDevPtr dev2 = factory->getResources()->getResourceByName(TESTCARD2);
    CPPUNIT_ASSERT(dev2);
    IDevPtr dev3 = factory->getResources()->getResourceByName(TESTCARD3);
    CPPUNIT_ASSERT(dev3);

    // some base info
    CPPUNIT_ASSERT(dev1->getIntParam(klk::dev::ADAPTER) == 0);
    CPPUNIT_ASSERT(dev1->getStringParam(klk::dev::NAME) == TESTCARD1);
    CPPUNIT_ASSERT(dev2->getIntParam(klk::dev::ADAPTER) == 1);
    CPPUNIT_ASSERT(dev2->getStringParam(klk::dev::NAME) == TESTCARD2);
    CPPUNIT_ASSERT(dev3->getIntParam(klk::dev::ADAPTER) == 2);
    CPPUNIT_ASSERT(dev3->getStringParam(klk::dev::NAME) == TESTCARD3);
    CPPUNIT_ASSERT(dev1->getState() == klk::dev::WORK);
    CPPUNIT_ASSERT(dev2->getState() == klk::dev::WORK);
    CPPUNIT_ASSERT(dev3->getState() == klk::dev::IDLE);

    // no any stat info set
    CPPUNIT_ASSERT(dev1->getIntParam(klk::dev::HASLOCK) == 0);
    CPPUNIT_ASSERT(dev1->getIntParam(klk::dev::SIGNAL) == 0);
    CPPUNIT_ASSERT(dev1->getIntParam(klk::dev::SNR) == 0);
    CPPUNIT_ASSERT(dev1->getIntParam(klk::dev::UNC) == 0);
    CPPUNIT_ASSERT(dev1->getIntParam(klk::dev::BER) == 0);

    CPPUNIT_ASSERT(dev2->getIntParam(klk::dev::HASLOCK) == 0);
    CPPUNIT_ASSERT(dev2->getIntParam(klk::dev::SIGNAL) == 0);
    CPPUNIT_ASSERT(dev2->getIntParam(klk::dev::SNR) == 0);
    CPPUNIT_ASSERT(dev2->getIntParam(klk::dev::UNC) == 0);
    CPPUNIT_ASSERT(dev2->getIntParam(klk::dev::BER) == 0);

    // check adapter settings (taht devs was tuned with DVB::doTune)
    CPPUNIT_ASSERT(dev1->getIntParam(klk::dev::FREQUENCY) == TEST_FREQUENCY1);
    CPPUNIT_ASSERT(dev2->getIntParam(klk::dev::FREQUENCY) == TEST_FREQUENCY2);
    CPPUNIT_ASSERT(dev3->hasParam(klk::dev::FREQUENCY) == false);
}

// The unit test itself
void TestDVB::testScan()
{
    test::printOut("\nDVB scan test ... ");

    // start scanning
    // start the processing
    IMessagePtr start_msg =
        m_msgfactory->getMessage(TEST_SCAN_MSG_ID);
    CPPUNIT_ASSERT(start_msg);
    // let check thread to be started
    sleep(10);
    m_modfactory->sendMessage(start_msg);
    sleep(5);

    klk::Result rc = getModule<TestHelpModule>(TEST_MOD_ID)->getResult();
    CPPUNIT_ASSERT(rc == klk::OK);

    // test info at DVB module

    // test devices
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);
    IDevPtr dev1 = factory->getResources()->getResourceByName(TESTCARD1);
    IDevPtr dev2 = factory->getResources()->getResourceByName(TESTCARD2);
    IDevPtr dev3 = factory->getResources()->getResourceByName(TESTCARD3);
    CPPUNIT_ASSERT(dev1->getState() == klk::dev::IDLE);
    CPPUNIT_ASSERT(dev2->getState() == klk::dev::IDLE);
    CPPUNIT_ASSERT(dev3->getState() == klk::dev::WORK);
    CPPUNIT_ASSERT(dev3->getStringParam(klk::dev::DVBACTIVITY) ==
                   klk::dev::SCANING);


    // stop the scanning
    IMessagePtr stop_msg = m_msgfactory->getMessage(msg::id::DVBSTOP);
    BOOST_ASSERT(stop_msg);
    stop_msg->setData(msg::key::DVBACTIVITY, klk::dev::SCANING);
    stop_msg->setData(msg::key::RESOURCE,
                      dev3->getStringParam(klk::dev::UUID));
    m_modfactory->sendMessage(stop_msg);
    // wait for a while
    sleep(5);
    rc = getModule<TestHelpModule>(TEST_MOD_ID)->getResult();
    CPPUNIT_ASSERT(rc == klk::OK);

    // check internal state
    CPPUNIT_ASSERT(dev3->getState() == klk::dev::IDLE);
}
