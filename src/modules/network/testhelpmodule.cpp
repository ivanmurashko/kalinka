/**
   @file testhelpmodule.cpp
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
   - 2008/11/07 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testhelpmodule.h"
#include "log.h"
#include "defines.h"
#include "testdefines.h"
#include "exception.h"
#include "messages.h"
#include "testutils.h"
#include "db.h"

using namespace klk;
using namespace klk::net;

//
// TestHelpModule class
//


// Constructor
TestHelpModule::TestHelpModule(IFactory *factory) :
    test::HelpModule(factory)
{
}

// Test instance
void TestHelpModule::doTest()
{
    doMainTest();
    doTestCleanup();
}

// Do the test
void TestHelpModule::doMainTest()
{
    IMessageFactory* msgfactory = getFactory()->getMessageFactory();
    BOOST_ASSERT(msgfactory);
    IMessagePtr request = msgfactory->getMessage(msg::id::NETSTART);
    BOOST_ASSERT(request);

    // do all tests
    doTest1(msgfactory, request);
    doTest2(msgfactory, request);
    doTest3(msgfactory, request);
}

// Do the cleanup test
void TestHelpModule::doTestCleanup()
{
    IMessageFactory* msgfactory = getFactory()->getMessageFactory();
    BOOST_ASSERT(msgfactory);

    klk_log(KLKLOG_DEBUG, "Start net clean test 1");
    IMessagePtr request = msgfactory->getMessage(msg::id::NETSTART);
    BOOST_ASSERT(request);
    request->setData(msg::key::MODINFOUUID, TESTROUTE1_UUID);
    IMessagePtr response1;
    sendSyncMessage(request, response1);
    // test response
    // not allowed already tuned
    BOOST_ASSERT(response1);
    BOOST_ASSERT(response1->getValue(msg::key::STATUS) ==
                    msg::key::FAILED);

    // restart NET module
    getFactory()->getModuleFactory()->unload(MODID);
    getFactory()->getModuleFactory()->load(MODID);
    // wait unitil db info will be read
    // FIXME!!!
    sleep(2);

    // now it's allowed
    IMessagePtr response2;
    sendSyncMessage(request, response2);
    // test response
    BOOST_ASSERT(response2);
    BOOST_ASSERT(response2->getValue(msg::key::STATUS) == msg::key::OK);
    BOOST_ASSERT(response2->getValue(msg::key::NETHOST) ==
                    TESTROUTE1_ADDR);
    BOOST_ASSERT(response2->getValue(msg::key::NETPORT) ==
                    TESTROUTE1_PORT);
    BOOST_ASSERT(response2->getValue(msg::key::NETPROTO) ==
                    TCPIP);
}

// Main test with ok requests
void TestHelpModule::doTest1(IMessageFactory* msgfactory,
                             const IMessagePtr& request)
{
    klk_log(KLKLOG_DEBUG, "Start net test 1");
    request->setData(msg::key::MODINFOUUID, TESTROUTE1_UUID);

    BOOST_ASSERT(getLockState(TESTROUTE1_UUID) == 0);

    IMessagePtr response1;
    sendSyncMessage(request, response1);
    // test response
    BOOST_ASSERT(response1);
    BOOST_ASSERT(response1->getValue(msg::key::STATUS) == msg::key::OK);
    BOOST_ASSERT(response1->getValue(msg::key::NETHOST) ==
                    TESTROUTE1_ADDR);
    BOOST_ASSERT(response1->getValue(msg::key::NETPORT) ==
                    TESTROUTE1_PORT);
    BOOST_ASSERT(response1->getValue(msg::key::NETPROTO) ==
                    TCPIP);
    BOOST_ASSERT(getLockState(TESTROUTE1_UUID) == 1);


    IMessagePtr response2;
    request->setData(msg::key::MODINFOUUID, TESTROUTE2_UUID);
    BOOST_ASSERT(getLockState(TESTROUTE2_UUID) == 0);
    sendSyncMessage(request, response2);
    // test response
    BOOST_ASSERT(response2);
    BOOST_ASSERT(response2->getValue(msg::key::STATUS) == msg::key::OK);
    BOOST_ASSERT(response2->getValue(msg::key::NETHOST) ==
                    TESTROUTE2_ADDR);
    BOOST_ASSERT(response2->getValue(msg::key::NETPORT) ==
                    TESTROUTE2_PORT);
    BOOST_ASSERT(response2->getValue(msg::key::NETPROTO) ==
                    UDP);
    BOOST_ASSERT(getLockState(TESTROUTE1_UUID) == 1);

}

// Test with wrong requests
void TestHelpModule::doTest2(IMessageFactory* msgfactory,
                             const IMessagePtr& request)
{
    klk_log(KLKLOG_DEBUG, "Start net test 2");
    // wrong request
    IMessagePtr response3;
    request->setData(msg::key::MODINFOUUID,
                     "2c12b8ff-ce5f-4eb3-9ffa-658c1612d02d");
    sendSyncMessage(request, response3);
    // test response
    BOOST_ASSERT(response3);
    BOOST_ASSERT(response3->getValue(msg::key::STATUS) ==
                    msg::key::FAILED);


    // wrong request
    BOOST_ASSERT(getLockState(TESTROUTE1_UUID) == 1);
    IMessagePtr response4;
    request->setData(msg::key::MODINFOUUID, TESTROUTE1_UUID);
    sendSyncMessage(request, response4);
    // test response
    BOOST_ASSERT(response4);
    BOOST_ASSERT(response4->getValue(msg::key::STATUS) ==
                    msg::key::FAILED);

    // del it
    IMessagePtr response5;
    BOOST_ASSERT(getLockState(TESTROUTE1_UUID) == 1);
    IMessagePtr request_del = msgfactory->getMessage(msg::id::NETSTOP);
    BOOST_ASSERT(request_del);
    request_del->setData(msg::key::MODINFOUUID, TESTROUTE1_UUID);
    sendSyncMessage(request_del, response5);
    // test response
    BOOST_ASSERT(response5);
    BOOST_ASSERT(response5->getValue(msg::key::STATUS) == msg::key::OK);
    BOOST_ASSERT(getLockState(TESTROUTE1_UUID) == 0);

    // NOW WE CAN ADD IT AGAIN
    IMessagePtr response6;
    request->setData(msg::key::MODINFOUUID, TESTROUTE1_UUID);
    sendSyncMessage(request, response6);
    // test response
    BOOST_ASSERT(response6);
    BOOST_ASSERT(response6->getValue(msg::key::STATUS) ==
                        msg::key::OK);
    BOOST_ASSERT(response6->getValue(msg::key::NETHOST) ==
                    TESTROUTE1_ADDR);
    BOOST_ASSERT(response6->getValue(msg::key::NETPORT) ==
                    TESTROUTE1_PORT);
    BOOST_ASSERT(response6->getValue(msg::key::NETPROTO) ==
                    TCPIP);

}

// Test with request for routes from external mediaservers
void TestHelpModule::doTest3(IMessageFactory* msgfactory,
                             const IMessagePtr& request)
{
    klk_log(KLKLOG_DEBUG, "Start net test 3 (data from another server)");
    IMessagePtr response1;
    request->setData(msg::key::MODINFOUUID, TESTROUTE4_UUID);
    sendSyncMessage(request, response1);
    // test response
    BOOST_ASSERT(response1);
    // from another network thus should not be accepted by the mediaserver
    BOOST_ASSERT(response1->getValue(msg::key::STATUS) ==
                    msg::key::FAILED);


    // it's locked now
    BOOST_ASSERT(getLockState(TESTROUTE3_UUID) == 1);

    // the request should be failed because the route is locked by another
    // mediaserver
    IMessagePtr response2;
    request->setData(msg::key::MODINFOUUID, TESTROUTE3_UUID);
    sendSyncMessage(request, response2);
    BOOST_ASSERT(response2);
    BOOST_ASSERT(response2->getValue(msg::key::STATUS) == msg::key::FAILED);


    // it should be accepted because gotten from the same network
    // the resource will be freed after some time
    sleep(LOCK_UPDATE_INTERVAL * 2 + 2);
    BOOST_ASSERT(getLockState(TESTROUTE3_UUID) == 0);
    IMessagePtr response3;
    sendSyncMessage(request, response3);
    BOOST_ASSERT(response3);
    BOOST_ASSERT(response3->getValue(msg::key::STATUS) == msg::key::OK);
    BOOST_ASSERT(response3->getValue(msg::key::NETHOST) ==
                    TESTROUTE3_ADDR);
    BOOST_ASSERT(response3->getValue(msg::key::NETPORT) ==
                    TESTROUTE3_PORT);
    BOOST_ASSERT(response3->getValue(msg::key::NETPROTO) ==
                    UDP);
    BOOST_ASSERT(getLockState(TESTROUTE3_UUID) == 1);

    // keep it locked?
    sleep(LOCK_UPDATE_INTERVAL * 2 + 2);
    BOOST_ASSERT(getLockState(TESTROUTE3_UUID) == 1);

    // but info can be gotten
    IMessagePtr response4;
    IMessagePtr request_info = msgfactory->getMessage(msg::id::NETINFO);
    BOOST_ASSERT(request_info);
    request_info->setData(msg::key::MODINFOUUID, TESTROUTE3_UUID);
    sendSyncMessage(request_info, response4);
    BOOST_ASSERT(response4);
    BOOST_ASSERT(response4->getValue(msg::key::STATUS) == msg::key::OK);
    BOOST_ASSERT(response4->getValue(msg::key::NETHOST) ==
                    TESTROUTE3_ADDR);
    BOOST_ASSERT(response4->getValue(msg::key::NETPORT) ==
                    TESTROUTE3_PORT);
    BOOST_ASSERT(response4->getValue(msg::key::NETPROTO) == UDP);

}

// Retrives lock state for specified route
int TestHelpModule::getLockState(const std::string& route)
{
    // `klk_network_route_getlock` (
    // IN route VARCHAR(40),
    // OUT return_value INT

    db::Parameters params;
    params.add("@route", route);
    params.add("@return_value");
    db::DB db(getFactory());
    db.connect();
    db::Result result = db.callSimple(
        "klk_network_route_getlock",
        params);

    return result["@return_value"].toInt();
}
