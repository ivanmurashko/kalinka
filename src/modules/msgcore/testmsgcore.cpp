/**
   @file testmsgcore.cpp
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

#include <memory>

#include "testmsgcore.h"
#include "modfactory.h"
#include "log.h"
#include "testfactory.h"
#include "utils.h"
#include "testid.h"
#include "testhelpmodule.h"
#include "defines.h"
#include "testutils.h"
#include "commontraps.h"
#include "testdepstart.h"

using namespace klk::msgcore;

// The function inits module's unit test
void klk_module_test_init()
{
    // main test
    const std::string TEST_MAIN = MODNAME + "/main";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestMsgCore,
                                          TEST_MAIN);
    CPPUNIT_REGISTRY_ADD(TEST_MAIN, MODNAME);

    // Dependency test
    const std::string TEST_DEP = MODNAME + "/dep";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestDependency,
                                          TEST_DEP);
    CPPUNIT_REGISTRY_ADD(TEST_DEP, MODNAME);

    CPPUNIT_REGISTRY_ADD(MODNAME, klk::test::ALL);
}

//
// TestMsgCore class
//

// Constructor
TestMsgCore::TestMsgCore() :
    test::TestModule(), m_module4async(), m_module4sync()
{
}

// Destructor
void TestMsgCore::tearDown()
{
    // module destructors unregister messages
    m_module4sync.reset();
    m_module4async.reset();
    test::TestModule::tearDown();
}


// Loads modules
void TestMsgCore::loadModules()
{
    // init test data
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);

    CPPUNIT_ASSERT(m_module4async == NULL);
    this->addDebug<TestModule4ASync>();
    m_module4async = this->getModule<TestModule4ASync>(TEST_MOD4ASYNC_ID);
    CPPUNIT_ASSERT(m_module4async);

    CPPUNIT_ASSERT(m_module4sync == NULL);
    this->addDebug<TestModule4Sync>();
    m_module4sync = this->getModule<TestModule4Sync>(TEST_MOD4SYNC_ID);
    CPPUNIT_ASSERT(m_module4sync);

    // load modules (start them in separate threads)
    m_modfactory->load(msgcore::MODID);
    m_modfactory->load(TEST_MOD4ASYNC_ID);
    m_modfactory->load(TEST_MOD4SYNC_ID);

    // wait for awhile until it will be loaded
    sleep(2);

    // send clear async message to all
    CPPUNIT_ASSERT(m_msgfactory);
    IMessagePtr clear_msg = m_msgfactory->getMessage(TEST_MSG4CLEAR_ID);
    CPPUNIT_ASSERT(clear_msg);

    m_modfactory->sendMessage(clear_msg);

    // wait for awhile until it will be processed
    sleep(2);
}

// The async. messages test
void TestMsgCore::testASync()
{
    test::printOut("\nMessaging core async. test ...");

    CPPUNIT_ASSERT(m_module4async->getCount() == 0);

    IMessagePtr test_msg = m_msgfactory->getMessage(TEST_MSG4ASYNC_ID);
    CPPUNIT_ASSERT(test_msg);

    // test itself
    CPPUNIT_ASSERT(m_module4async->getCount() == 0);
    m_modfactory->sendMessage(test_msg);
    sleep(2);
    // now we should get it
    CPPUNIT_ASSERT(m_module4async->getCount() == 1);
    m_modfactory->sendMessage(test_msg);
    sleep(2);
    // now we should get it again
    CPPUNIT_ASSERT(m_module4async->getCount() == 2);
}

// The async. messages test
void TestMsgCore::testSync()
{
    test::printOut("\nMessaging core sync. test ...");

    IMessagePtr test_msg = m_msgfactory->getMessage(TEST_MSG4SYNCSTART_ID);
    CPPUNIT_ASSERT(test_msg);

    m_modfactory->sendMessage(test_msg);

    // wait for awile
    sleep(4*TIMEINTERVAL4SYNCRES + 10);

    IMessagePtr test_response = m_module4sync->getResponse();
    CPPUNIT_ASSERT(test_response);

    std::string response_data = test_response->getValue("");
    CPPUNIT_ASSERT(response_data == TEST_MSG_DATA);

    // we should got one trap that module
    // TEST_MOD4ASYNC_ID does not respond
    test::TrapInfoVector result = getTraps();
    CPPUNIT_ASSERT(result.empty() == false);
    CPPUNIT_ASSERT(result.size() == 1);
    CPPUNIT_ASSERT(result[0].getValue() == TEST_MOD4ASYNC_ID);
    CPPUNIT_ASSERT(test::IsOIDMatch()(result[0], snmp::MODULE_DNT_RESPONDE) ==
                   true);
}
