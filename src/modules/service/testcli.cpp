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
   - 2010/08/01 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testcli.h"
#include "defines.h"
#include "cliutils.h"
#include "testutils.h"
#include "clicommands.h"

// helper module
#include "adapter/defines.h"
#include "adapter/messagesprotocol.h"

using namespace klk;
using namespace klk::srv;



//
// TestCLI class
//

/// Constructor
TestCLI::TestCLI() : test::TestModule()
{
}

// Loads all necessary modules at setUp()
void TestCLI::loadModules()
{
    CPPUNIT_ASSERT(m_modfactory);
    m_modfactory->load(MODID);
    loadAdapter();
}

/// The unit test for CLI tests
void TestCLI::test()
{
    klk::test::printOut("\nService CLI test ... ");

    IMessagePtr in =
        m_msgfactory->getMessage(INFO_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    // empty info this sets CLI type for processing
    cli::ParameterVector params;
    cli::Utils::setProcessParams(in, params);
    adapter::MessagesProtocol proto(klk::test::Factory::instance());
    IMessagePtr out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    in = m_msgfactory->getMessage(INFO_MOD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    // empty info this sets CLI type for processing
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    in = m_msgfactory->getMessage(INFO_APP_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    // empty info this sets CLI type for processing
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    in = m_msgfactory->getMessage(LOAD_MOD_COMMAND_ID);
    // empty info this sets CLI type for processing
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    in = m_msgfactory->getMessage(LOAD_APP_COMMAND_ID);
    // empty info this sets CLI type for processing
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    // FIXME!!! add more tests
}
