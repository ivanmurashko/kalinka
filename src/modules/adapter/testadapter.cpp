/**
   @file testadapter.cpp
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
   - 2008/11/10 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testadapter.h"
#include "defines.h"
#include "testid.h"
#include "testhelpmodule.h"
#include "testfactory.h"
#include "messagesprotocol.h"
#include "resourcesprotocol.h"
#include "testutils.h"
#include "adapter.h"

using namespace klk;
using namespace klk::adapter;

// The function inits module's unit test
void klk_module_test_init()
{
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestAdapter,
                                          MODNAME);
    CPPUNIT_REGISTRY_ADD(MODNAME, test::ALL);
}

//
// TestAdapter class
//


// Constructor
TestAdapter::TestAdapter() :
    test::TestModuleWithDB("")
{
}

// Loads all necessary modules at setUp()
void TestAdapter::loadModules()
{
    CPPUNIT_ASSERT(m_modfactory);

    loadAdapter();

    // add helper module
    this->addDebug<TestHelpModule>();

    // load helper module
    m_modfactory->load(TEST_MOD_ID);
}

// The unit test for startup/stop
void TestAdapter::testStartStop()
{
    test::printOut("\nAdapter start/stop test ... ");

    IFactory* factory = test::Factory::instance();

    // sleep for awhile
    sleep(4);

    // check the adapter number was loaded
    IModulePtr module = m_modfactory->getModule(klk::adapter::MODID);
    CPPUNIT_ASSERT(module);
    CPPUNIT_ASSERT(module->getName() == klk::adapter::MODNAME);
    boost::shared_ptr<klk::adapter::Adapter> adapter =
        boost::dynamic_pointer_cast<klk::adapter::Adapter>(module);
    CPPUNIT_ASSERT(adapter);
    CPPUNIT_ASSERT(adapter->m_adapters.size() == 4);

    // now check the DB settings
    // retrive route information
    db::DB db(factory);
    db.connect();

    // messages object
    db::Parameters params_msg;
    params_msg.add("@host", db.getHostUUID());
    params_msg.add("@module", factory->getMainModuleId());
    params_msg.add("@object", obj::MESSAGES);
    params_msg.add("@endpoint");
    params_msg.add("@return_value");

    db::Result res = db.callSimple("klk_adapter_get", params_msg);
    CPPUNIT_ASSERT(res["@return_value"].toInt() == 0);
    CPPUNIT_ASSERT(res["@endpoint"].isNull() == false);
    CPPUNIT_ASSERT(res["@endpoint"].toString().empty() == false);

    // check connection to messages RPC
    MessagesProtocol msg_proto(factory);
    CPPUNIT_ASSERT (msg_proto.checkConnection() == klk::OK);

    // resources object
    db::Parameters params_res;
    params_res.add("@host", db.getHostUUID());
    params_res.add("@module", factory->getMainModuleId());
    params_res.add("@object", obj::RESOURCES);
    params_res.add("@endpoint");
    params_res.add("@return_value");

    res = db.callSimple("klk_adapter_get", params_res);
    CPPUNIT_ASSERT(res["@return_value"].toInt() == 0);
    CPPUNIT_ASSERT(res["@endpoint"].isNull() == false);
    CPPUNIT_ASSERT(res["@endpoint"].toString().empty() == false);

    // check connection to resources RPC
    ResourcesProtocol res_proto(factory);
    CPPUNIT_ASSERT (res_proto.checkConnection() == klk::OK);

    // stop all
    m_modfactory->unload(MODID);
    sleep(2);

    adapter =
        boost::dynamic_pointer_cast<Adapter>(m_modfactory->getModule(MODID));
    CPPUNIT_ASSERT(adapter != NULL);
    //CPPUNIT_ASSERT(adapter->m_adapters.empty());

    // check that DB has been updated

    // messages object
    res = db.callSimple("klk_adapter_get", params_msg);
    CPPUNIT_ASSERT(res["@return_value"].toInt() == 0);
    CPPUNIT_ASSERT(res["@endpoint"].isNull() == true);

    // resources object
    res = db.callSimple("klk_adapter_get", params_res);
    CPPUNIT_ASSERT(res["@return_value"].toInt() == 0);
    CPPUNIT_ASSERT(res["@endpoint"].isNull() == true);
}



// The unit test first start
void TestAdapter::test1()
{
    test::printOut("\n1st. Adapter test ... ");
    test();
}

// The unit test second start
void TestAdapter::test2()
{
    test::printOut("\n2d. Adapter test ... ");
    test();
}

// The unit test itself
void TestAdapter::test()
{
    MessagesProtocol proto(test::Factory::instance());

    // test async
    bool res = getModule<TestHelpModule>(TEST_MOD_ID)->getASyncResult();
    CPPUNIT_ASSERT(res == false);
    IMessagePtr async_req = m_msgfactory->getMessage(TEST_MSG_ASYNC_ID);
    proto.sendASync(async_req);
    sleep(2);
    res = getModule<TestHelpModule>(TEST_MOD_ID)->getASyncResult();
    CPPUNIT_ASSERT(res == true);

    // test sync
    IMessagePtr sync_req = m_msgfactory->getMessage(TEST_MSG_SYNC_ID);
    // set additional fields
    sync_req->setData("req1", "request1");
    sync_req->setData("req2", "request2");
    StringList listdata;
    listdata.push_back("data1");
    listdata.push_back("data2");
    sync_req->setData("list", listdata);
    IMessagePtr sync_res = proto.sendSync(sync_req);
    CPPUNIT_ASSERT(sync_res);
    CPPUNIT_ASSERT(sync_res->getValue(msg::key::STATUS) == msg::key::OK);
    CPPUNIT_ASSERT(sync_res->getValue("res1") == "response1");
    CPPUNIT_ASSERT(sync_res->getValue("res2") == "response2");
    CPPUNIT_ASSERT(sync_res->getValue("res3") == "response3");
    CPPUNIT_ASSERT(sync_res->getList("list") == listdata);

    // test sync wrong
    IMessagePtr sync_req_wrong =
        m_msgfactory->getMessage(TEST_MSG_SYNC_WRONG_ID);
    // set additional fields
    sync_req_wrong->setData("req1", "request1");
    IMessagePtr sync_res_wrong = proto.sendSync(sync_req_wrong);
    CPPUNIT_ASSERT(sync_res_wrong);
    CPPUNIT_ASSERT(sync_res_wrong->getValue(msg::key::STATUS) ==
                   msg::key::FAILED);

}
