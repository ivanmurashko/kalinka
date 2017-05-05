/**
   @file testcli.cpp
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
   - 2009/03/04 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "routecommand.h"
#include "defines.h"
#include "testdefines.h"
#include "testfactory.h"
#include "netdev.h"
#include "testcli.h"
#include "cliutils.h"
#include "testutils.h"

// helper module
#include "../checkdb/defines.h"
#include "../adapter/messagesprotocol.h"

using namespace klk;
using namespace klk::net;

//
// TestNetwork class
//


// Constructor
TestCLI::TestCLI() :
    test::TestModuleWithDB("testnetwork.sql")
{
}

// Loads all necessary modules at setUp()
void TestCLI::loadModules()
{
    CPPUNIT_ASSERT(m_modfactory);

    m_modfactory->load(MODID);

    // load adapter
    loadAdapter();

    // init lo dev
    test::Factory::instance()->getResources()->updateDB(dev::NET);
}

// The unit test itself
void TestCLI::testMain()
{
    test::printOut("\nNetwork cli test ... ");

    // variables
    adapter::MessagesProtocol proto(test::Factory::instance());
    IMessagePtr out, in;

    // valid route names
    const std::string valid_name1("route_unique1");
    const std::string valid_name2("route_unique2");


    // not valid dev name
    in = m_msgfactory->getMessage(ROUTE_ADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params(4);
    params[0] = valid_name1;
    params[1] = "eth1_invalid";
    params[2] = "127.0.0.1";
    params[3] = "2000";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // not valid dev name (present at the db)
    in = m_msgfactory->getMessage(ROUTE_ADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    params[0] = valid_name1;
    params[1] = TESTDEV_NAME;
    params[2] = TESTDEV_ADDR;
    params[3] = "2000";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // not valid IP address
    in = m_msgfactory->getMessage(ROUTE_ADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    const std::string not_valid_route = "192.168.0.1";
    testList(not_valid_route, false);
    params[0] = valid_name1;
    params[1] = LOOPBACK_NAME;
    params[2] = not_valid_route;
    params[3] = "2000";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);
    testList(not_valid_route, false);

    // valid IP multicast
    in = m_msgfactory->getMessage(ROUTE_ADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    const std::string multicast_route = "234.0.1.1";
    testList(multicast_route, false);
    params[0] = valid_name1;
    params[1] = LOOPBACK_NAME;
    params[2] = multicast_route;
    params[3] = "2000";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);
    testList(multicast_route, true);

    // valid route
    const std::string valid_route = "127.0.0.1";

    // add a simple valid
    in = m_msgfactory->getMessage(ROUTE_ADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    testList(valid_route, false);
    params[0] = valid_name2;
    params[1] = LOOPBACK_NAME;
    params[2] = valid_route;
    params[3] = "2000";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);
    testList(valid_route, true);

    // not valid route name (present at the db)
    in = m_msgfactory->getMessage(ROUTE_ADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    params[0] = valid_name2;
    params[1] = LOOPBACK_NAME;
    params[2] = valid_route;
    params[3] = "4321";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // del valid route
    in = m_msgfactory->getMessage(ROUTE_DEL_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    testList(valid_route, true);
    cli::ParameterVector del_params(1);
    del_params[0] = valid_name2;
    cli::Utils::setProcessParams(in, del_params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);
    testList(valid_route, false);

    // del valid route again
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);
    testList(valid_route, false);

    in = m_msgfactory->getMessage(ROUTE_ADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    params[0] = valid_name2;
    params[1] = LOOPBACK_NAME;
    params[2] = valid_route;
    params[3] = "4321";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);
}

// Test that response have the specified route
void TestCLI::testList(const std::string& route, bool presence)
{
    // we should wait for DB updates before any check
    sleep(db::CHECKDBINTERVAL + 2);

    // variables
    adapter::MessagesProtocol proto(test::Factory::instance());
    // test addition of known command
    IMessagePtr in = m_msgfactory->getMessage(ROUTE_LIST_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params(1);
    params[0] = cli::ALL;
    cli::Utils::setProcessParams(in, params);

    IMessagePtr out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    const std::string response = out->getValue(msg::key::CLIRESULT);
    bool found = (response.find(route) != std::string::npos);
    if (presence)
    {
        CPPUNIT_ASSERT(found == true);
    }
    else
    {
        CPPUNIT_ASSERT(found == false);
    }
}
