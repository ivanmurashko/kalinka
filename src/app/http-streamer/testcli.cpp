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
   - 2009/03/20 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "outcmd.h"
#include "incmd.h"
#include "statcmd.h"

#include "defines.h"
#include "testdefines.h"
#include "testcli.h"
#include "cliutils.h"
#include "testutils.h"

// helper module
#include "adapter/messagesprotocol.h"
#include "checkdb/defines.h"
#include "service/messages.h"

using namespace klk;
using namespace klk::http;

//
// TestNetwork class
//


// Constructor
TestCLI::TestCLI() :
    test::TestModuleWithDB("testhttpstreamer_cli.sql")
{
}

// Loads all necessary modules at setUp()
void TestCLI::loadModules()
{
    CPPUNIT_ASSERT(m_modfactory);

    // load adapter
    loadAdapter();
    sleep(2);

    m_modfactory->load(MODID);
}

// The unit test itself
void TestCLI::testOut()
{
    test::printOut("\nHTTP streamer cli test (output info) ... ");

    IMessagePtr out, in;
    adapter::MessagesProtocol proto(test::Factory::instance());

    // add an ivalid route
    in = m_msgfactory->getMessage(OUT_SET_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params(1);
    params[0] = "invalid_route";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // now ok
    in = m_msgfactory->getMessage(OUT_SET_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    params[0] = TESTSERVERNAME;
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    sleep(db::CHECKDBINTERVAL + 2);
    testOutShow(TESTSERVERNAMEADD, false);
    testOutShow(TESTSERVERNAME, true);

    // try to change output route
    in = m_msgfactory->getMessage(OUT_SET_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    params[0] = TESTSERVERNAME;
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // now ok
    in = m_msgfactory->getMessage(OUT_SET_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    params[0] = TESTSERVERNAMEADD;
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    sleep(db::CHECKDBINTERVAL + 2);
    testOutShow(TESTSERVERNAMEADD, true);
    testOutShow(TESTSERVERNAME, false);

    // now should be able return back
    in = m_msgfactory->getMessage(OUT_SET_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    params[0] = TESTSERVERNAME;
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);
}

// Adds test output route
void TestCLI::setOutput()
{
    adapter::MessagesProtocol proto(test::Factory::instance());
    cli::ParameterVector params(1);
    params[0] = TESTSERVERNAME;
    IMessagePtr in = m_msgfactory->getMessage(OUT_SET_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::Utils::setProcessParams(in, params);
    IMessagePtr out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);
}


// The unit test itself
void TestCLI::testIn()
{
    test::printOut("\nHTTP streamer cli test (stat info) ... ");

    // prepare
    setOutput();
    setInput();

    // initial data
    sleep(db::CHECKDBINTERVAL);
    testStatShow(TESTPATH1, true);
    testStatShow(TESTPATH2, true);
}

// Adds initial test input routes
void TestCLI::setInput()
{
    IMessagePtr out, in;
    cli::ParameterVector params(3);
    adapter::MessagesProtocol proto(test::Factory::instance());

    // add two initial routes
    in = m_msgfactory->getMessage(IN_ADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    params[0] = TESTNAME1;
    params[1] = TESTPATH1;
    params[2] = "flv";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);
    params[0] = TESTNAME2;
    params[1] = TESTPATH2;
    params[2] = "flv";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    sleep(db::CHECKDBINTERVAL);
    testInShow(TESTNAME1, true);
    testInShow(TESTNAME2, true);
}


// The unit test for stat commands
void TestCLI::testStat()
{
    test::printOut("\nHTTP streamer cli test (in info) ... ");

    // prepare
    setOutput();

    // initial data
    sleep(db::CHECKDBINTERVAL);
    testInShow(TESTNAME1, false);
    testInShow(TESTNAME2, false);
    testInShow(TESTSERVERNAME, false);

    IMessagePtr out, in;
    cli::ParameterVector params(3);
    adapter::MessagesProtocol proto(test::Factory::instance());

    // add two initial routes
    setInput();
    sleep(db::CHECKDBINTERVAL);
    testInShow(TESTNAME1, true);
    testInShow(TESTNAME2, true);
    testInShow(TESTSERVERNAME, false);

    // try to wrong add
    in = m_msgfactory->getMessage(IN_ADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    params[0] = TESTNAME1;
    params[1] = TESTPATH1;
    params[2] = "flv";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // try valid del
    in = m_msgfactory->getMessage(IN_DEL_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector del_params(1);
    del_params[0] = TESTNAME1;
    cli::Utils::setProcessParams(in, del_params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);
    sleep(db::CHECKDBINTERVAL + 3);
    // check result
    testInShow(TESTNAME1, false);
    testInShow(TESTNAME2, true);

    // try the add again
    in = m_msgfactory->getMessage(IN_ADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    params[0] = TESTNAME1;
    params[1] = TESTPATH1;
    params[2] = "flv";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);
    sleep(db::CHECKDBINTERVAL + 3);
    // check result
    testInShow(TESTNAME1, true);
    testInShow(TESTNAME2, true);
}


// Test that response have the specified data
void TestCLI::testOutShow(const std::string& test, bool presence)
{
    // variables
    adapter::MessagesProtocol proto(test::Factory::instance());
    // test addition of known command
    IMessagePtr in = m_msgfactory->getMessage(OUT_SHOW_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    // empty info this sets CLI type for processing
    cli::ParameterVector params;
    cli::Utils::setProcessParams(in, params);
    // not valid IP address
    IMessagePtr out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    const std::string response = out->getValue(msg::key::CLIRESULT);
    bool found = (response.find(test) != std::string::npos);
    if (presence)
    {
        CPPUNIT_ASSERT(found == true);
    }
    else
    {
        CPPUNIT_ASSERT(found == false);
    }
}


// Test that response have the specified data
void TestCLI::testInShow(const std::string& test, bool presence)
{
    // variables
    adapter::MessagesProtocol proto(test::Factory::instance());
    // test addition of known command
    IMessagePtr in = m_msgfactory->getMessage(IN_SHOW_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    // empty info
    cli::ParameterVector params;
    cli::Utils::setProcessParams(in, params);
    // not valid IP address
    IMessagePtr out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    const std::string response = out->getValue(msg::key::CLIRESULT);
    bool found = (response.find(test) != std::string::npos);
    if (presence)
    {
        CPPUNIT_ASSERT(found == true);
    }
    else
    {
        CPPUNIT_ASSERT(found == false);
    }
}

// Test that response have the specified data
void TestCLI::testStatShow(const std::string& test, bool presence)
{
    // variables
    adapter::MessagesProtocol proto(test::Factory::instance());
    // test addition of known command
    IMessagePtr in = m_msgfactory->getMessage(STAT_COMMAND_ID);
    CPPUNIT_ASSERT(in);

    // input test
    cli::ParameterVector params(1);
    params[0] = STAT_COMMAND_PARAM_INPUT;
    cli::Utils::setProcessParams(in, params);
    IMessagePtr out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    std::string response = out->getValue(msg::key::CLIRESULT);
    bool found = (response.find(test) != std::string::npos);
    if (presence)
    {
        CPPUNIT_ASSERT(found == true);
    }
    else
    {
        CPPUNIT_ASSERT(found == false);
    }


    // output test
    params[0] = STAT_COMMAND_PARAM_OUTPUT;
    cli::Utils::setProcessParams(in, params);

    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    response = out->getValue(msg::key::CLIRESULT);
    found = (response.find(test) != std::string::npos);
    if (presence)
    {
        CPPUNIT_ASSERT(found == true);
    }
    else
    {
        CPPUNIT_ASSERT(found == false);
    }

}
