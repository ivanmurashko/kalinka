/**
   @file testsourcecli.cpp
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
   - 2009/11/08 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testsourcecli.h"
#include "exception.h"
#include "defines.h"
#include "testdefines.h"
#include "testutils.h"
#include "sourcecmd.h"
#include "cliutils.h"

// adapter module
#include "adapter/messagesprotocol.h"

using namespace klk;
using namespace klk::trans;
using namespace klk::trans::test;

//
// TestSourceCLI class
//

// Constructor
TestSourceCLI::TestSourceCLI() :
    klk::test::TestModuleWithDB("testtranscode_sourcecli.sql")
{
}

// Destructor
TestSourceCLI::~TestSourceCLI()
{
}

// The unit test itself
void TestSourceCLI::test()
{
    klk::test::printOut("\nTranscode CLI test (source info) ... ");
    testInvalid();
    testAdd();
    testDel();
}

// Loads all necessary modules at setUp()
void TestSourceCLI::loadModules()
{
    CPPUNIT_ASSERT(m_modfactory);

    // load adapter first
    // this is necessary because the main module (streamer)
    // is derived from klk::launcher::app::Module that
    // requires adapter to be run in constructor
    loadAdapter();

    m_modfactory->load(MODID);
}

// Tests invalid parameters
void TestSourceCLI::testInvalid()
{
    klk::test::printOut("\n\tInvalid test ...");

    // variables
    adapter::MessagesProtocol proto(klk::test::Factory::instance());
    IMessagePtr out, in;

    // not valid name
    in = m_msgfactory->getMessage(SOURCEADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params(3);
    params[0] = NETTYPENAME;
    params[1] = "invalid_net";
    params[2] = MEDIA_FLV;
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // invalid media type name
    params[0] = NETTYPENAME;
    params[1] = ROUTENAME;
    params[2] = "INVALID";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);
}

// Tests source add command
void TestSourceCLI::testAdd()
{
    klk::test::printOut("\n\tAdd sources test ...");
    // variables
    adapter::MessagesProtocol proto(klk::test::Factory::instance());
    IMessagePtr out, in;

    testShow(SOURCE_ROUTENAME, false);
    testShow(SOURCE_INPUTFILENAME, false);

    // valid route name
    in = m_msgfactory->getMessage(SOURCEADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params(3);
    params[0] = NETTYPENAME;
    params[1] = ROUTENAME;
    params[2] = MEDIA_FLV;
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    // test result
    testShow(SOURCE_ROUTENAME, true);
    testShow(SOURCE_INPUTFILENAME, false);

    // try to add it again (should failed)
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // test result (should not be changed)
    testShow(SOURCE_ROUTENAME, true);
    testShow(SOURCE_INPUTFILENAME, false);

    // valid file name
    params[0] = FILETYPENAME;
    params[1] = INPUTFILENAME;
    params[2] = MEDIA_FLV;
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    // test result
    testShow(SOURCE_ROUTENAME, true);
    testShow(SOURCE_INPUTFILENAME, true);
}

// Tests source delete command
void TestSourceCLI::testDel()
{
    klk::test::printOut("\n\tDelete sources test ...");

    // variables
    adapter::MessagesProtocol proto(klk::test::Factory::instance());
    IMessagePtr out, in;

    testShow(SOURCE_ROUTENAME, true);
    testShow(SOURCE_INPUTFILENAME, true);

    // valid route name
    in = m_msgfactory->getMessage(SOURCEDEL_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params(1);
    params[0] = SOURCE_ROUTENAME;
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    // test result
    testShow(SOURCE_ROUTENAME, false);
    testShow(SOURCE_INPUTFILENAME, true);

    // try to del it again (should failed)
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // test result (should not be changed)
    testShow(SOURCE_ROUTENAME, false);
    testShow(SOURCE_INPUTFILENAME, true);

    // valid file name
    params[0] = SOURCE_INPUTFILENAME;
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    // test result
    testShow(SOURCE_ROUTENAME, false);
    testShow(SOURCE_INPUTFILENAME, false);
}

// Test source in the show list
void TestSourceCLI::testShow(const std::string& source, bool presence)
{
    // variables
    adapter::MessagesProtocol proto(klk::test::Factory::instance());
    // test addition of known command
    IMessagePtr in = m_msgfactory->getMessage(SOURCESHOW_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params;
    cli::Utils::setProcessParams(in, params);

    IMessagePtr out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    const std::string response = out->getValue(msg::key::CLIRESULT);
    bool found = (response.find(source) != std::string::npos);
    if (presence)
    {
        CPPUNIT_ASSERT(found == true);
    }
    else
    {
        CPPUNIT_ASSERT(found == false);
    }
}
