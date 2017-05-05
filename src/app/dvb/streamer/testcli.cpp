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
   - 2009/07/05 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "stationaddcmd.h"
#include "stationdelcmd.h"
#include "showcmd.h"

#include "defines.h"
#include "testdefines.h"
#include "testfactory.h"
#include "testcli.h"
#include "cliutils.h"
#include "testutils.h"

// helper module
#include "checkdb/defines.h"
#include "adapter/messagesprotocol.h"

using namespace klk;
using namespace klk::dvb::stream;

//
// TestNetwork class
//


// Constructor
TestCLI::TestCLI() :
    test::TestModuleWithDB("testdvbstreamer.sql")
{
}

// Loads all necessary modules at setUp()
void TestCLI::loadModules()
{
    // load adapter first
    // this is necessary because the main module (streamer)
    // is derived from klk::launcher::app::Module that
    // requires adapter to be run in constructor
    loadAdapter();

    CPPUNIT_ASSERT(m_modfactory);
    m_modfactory->load(MODID);
}

// The unit test itself
void TestCLI::testMain()
{
    test::printOut("\nDVB Streamer CLI test ... ");

    test::printOut("\n\tInvalid test ... ");
    testInvalid();

    test::printOut("\n\tAdd test ... ");
    testAdd();

    test::printOut("\n\tDel test ... ");
    testDel();
}

// Test that response have the specified station
void TestCLI::testShow(const std::string& station, bool presence)
{
    // we should wait for DB updates before any check
    sleep(db::CHECKDBINTERVAL + 2);

    testAss(station, presence);
    testNotAss(station, !presence);
}

// test assigned channels
void TestCLI::testAss(const std::string& station, bool presence)
{
    // variables
    adapter::MessagesProtocol proto(test::Factory::instance());
    // test addition of known command
    IMessagePtr in = m_msgfactory->getMessage(STATION_SHOW_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params;
    cli::Utils::setProcessParams(in, params);

    IMessagePtr out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    const std::string response = out->getValue(msg::key::CLIRESULT);
    bool found = (response.find(station) != std::string::npos);
    if (presence)
    {
        CPPUNIT_ASSERT(found == true);
    }
    else
    {
        CPPUNIT_ASSERT(found == false);
    }
}


// Test that response have the specified channel
// at not assigned list
void TestCLI::testNotAss(const std::string& station, bool presence)
{
    // variables
    adapter::MessagesProtocol proto(test::Factory::instance());
    // test addition of known command
    IMessagePtr in = m_msgfactory->getMessage(NOT_ASSIGNED_SHOW_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params;
    cli::Utils::setProcessParams(in, params);

    IMessagePtr out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    const std::string response = out->getValue(msg::key::CLIRESULT);
    bool found = (response.find(station) != std::string::npos);
    if (presence)
    {
        CPPUNIT_ASSERT(found == true);
    }
    else
    {
        CPPUNIT_ASSERT(found == false);
    }
}


// Tests invalid data
void TestCLI::testInvalid()
{
    // variables
    adapter::MessagesProtocol proto(test::Factory::instance());
    IMessagePtr out, in;


    // not valid channel name
    in = m_msgfactory->getMessage(STATION_ADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params(3);
    params[0] = "invalid_channel";
    params[1] = TESTROUTE1;
    params[2] = "1";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // invalid route
    in = m_msgfactory->getMessage(STATION_ADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    params[0] = TESTSTATION1;
    params[1] = "ivalid_route";
    params[2] = "1";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // invalid del
    params.resize(1);
    in = m_msgfactory->getMessage(STATION_DEL_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    params[0] = "invalid_channel";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);
}

// Tests data add
void TestCLI::testAdd()
{
    testShow(TESTSTATION1, false);
    testShow(TESTSTATION2, false);

    // variables
    adapter::MessagesProtocol proto(test::Factory::instance());
    IMessagePtr out, in;

    // channel1
    in = m_msgfactory->getMessage(STATION_ADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params(3);
    params[0] = TESTSTATION1;
    params[1] = TESTROUTE1;
    params[2] = "1";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);
    testShow(TESTSTATION1, true);
    testShow(TESTSTATION2, false);

    // channel2 invalid (to already assigned route)
    in = m_msgfactory->getMessage(STATION_ADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    params[0] = TESTSTATION2;
    params[1] = TESTROUTE1;
    params[2] = "1";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);
    testShow(TESTSTATION1, true);
    testShow(TESTSTATION2, false);

    // route2 invalid (to already assigned channel)
    in = m_msgfactory->getMessage(STATION_ADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    params[0] = TESTSTATION1;
    params[1] = TESTROUTE2;
    params[2] = "1";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);
    testShow(TESTSTATION1, true);
    testShow(TESTSTATION2, false);

    // channel2/route2 valid
    in = m_msgfactory->getMessage(STATION_ADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    params[0] = TESTSTATION2;
    params[1] = TESTROUTE2;
    params[2] = "1";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);
    testShow(TESTSTATION1, true);
    testShow(TESTSTATION2, true);
}

// Tests data del
void TestCLI::testDel()
{
    testShow(TESTSTATION1, true);
    testShow(TESTSTATION2, true);

    // variables
    adapter::MessagesProtocol proto(test::Factory::instance());
    IMessagePtr out, in;

    // channel1
    in = m_msgfactory->getMessage(STATION_DEL_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params(1);
    params[0] = TESTSTATION1;
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);
    testShow(TESTSTATION1, false);
    testShow(TESTSTATION2, true);

    // invalid (try to delete it again)
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);
    testShow(TESTSTATION1, false);
    testShow(TESTSTATION2, true);


    // channel2 valid
    in = m_msgfactory->getMessage(STATION_DEL_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    params[0] = TESTSTATION2;
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);
    testShow(TESTSTATION1, false);
    testShow(TESTSTATION2, false);
}


