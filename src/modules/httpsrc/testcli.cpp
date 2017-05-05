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
   - 2009/12/20 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testcli.h"
#include "exception.h"
#include "defines.h"
#include "testdefines.h"
#include "testutils.h"
#include "cmd.h"
#include "cliutils.h"

// helper module
#include "adapter/messagesprotocol.h"

using namespace klk;
using namespace klk::httpsrc;

//
// TestCLI class
//

// Constructor
TestCLI::TestCLI() :
    klk::test::TestModuleWithDB("")
{
}

// Destructor
TestCLI::~TestCLI()
{
}

// The unit test itself
void TestCLI::test()
{
    klk::test::printOut("\nHTTP source CLI test ... ");
    testInvalid();
    testAdd();
    testDel();
}

// Loads all necessary modules at setUp()
void TestCLI::loadModules()
{
    CPPUNIT_ASSERT(m_modfactory);

    m_modfactory->load(MODID);

    // load adapter
    loadAdapter();
}

// Tests invalid parameters
void TestCLI::testInvalid()
{
    klk::test::printOut("\n\tInvalid test ...");

    // variables
    adapter::MessagesProtocol proto(test::Factory::instance());
    IMessagePtr out, in;

    // not valid name
    in = m_msgfactory->getMessage(ADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params(2);
    params[0] = "source1";
    params[1] = "ftp://test.html";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);
}

// Tests  add command
void TestCLI::testAdd()
{
    klk::test::printOut("\n\tAdd test ...");
    // variables
    adapter::MessagesProtocol proto(klk::test::Factory::instance());
    IMessagePtr out, in;

    testShow(TEST_NAME1, false);
    testShow(TEST_NAME2, false);

    // valid route name
    in = m_msgfactory->getMessage(ADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params(4);
    params[0] = TEST_NAME1;
    params[1] = TEST_ADDR1;
    params[2] = TEST_LOGIN1;
    params[3] = TEST_PWD1;
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    // test result
    testShow(TEST_NAME1, true);
    testShow(TEST_ADDR1, true);
    testShow(TEST_NAME2, false);

    // try to add it again (should failed)
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // test result (should not be changed)
    testShow(TEST_NAME1, true);
    testShow(TEST_NAME2, false);

    // valid file name
    params[0] = TEST_NAME2;
    params[1] = TEST_ADDR2;
    params[2] = TEST_LOGIN2;
    params[3] = TEST_PWD2;
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    // test result
    testShow(TEST_NAME1, true);
    testShow(TEST_NAME2, true);
    testShow(TEST_ADDR2, true);
}

// Tests  delete command
void TestCLI::testDel()
{
    klk::test::printOut("\n\tDelete test ...");

    // variables
    adapter::MessagesProtocol proto(klk::test::Factory::instance());
    IMessagePtr out, in;

    testShow(TEST_NAME1, true);
    testShow(TEST_NAME2, true);

    // valid route name
    in = m_msgfactory->getMessage(DEL_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params(1);
    params[0] = TEST_NAME1;
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    // test result
    testShow(TEST_NAME1, false);
    testShow(TEST_NAME2, true);

    // try to del it again (should failed)
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // test result (should not be changed)
    testShow(TEST_NAME1, false);
    testShow(TEST_NAME2, true);

    // valid file name
    params[0] = TEST_NAME2;
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    // test result
    testShow(TEST_NAME1, false);
    testShow(TEST_NAME2, false);
}

// Test  in the show list
void TestCLI::testShow(const std::string& pattern, bool presence)
{
    // variables
    adapter::MessagesProtocol proto(klk::test::Factory::instance());
    // test addition of known command
    IMessagePtr in = m_msgfactory->getMessage(SHOW_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params;
    cli::Utils::setProcessParams(in, params);

    IMessagePtr out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    const std::string response = out->getValue(msg::key::CLIRESULT);
    bool found = (response.find(pattern) != std::string::npos);
    if (presence)
    {
        CPPUNIT_ASSERT(found == true);
    }
    else
    {
        CPPUNIT_ASSERT(found == false);
    }
}
