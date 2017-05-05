/**
   @file testtaskcli.cpp
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
   - 2009/11/30 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testtaskcli.h"
#include "exception.h"
#include "defines.h"
#include "testdefines.h"
#include "testutils.h"
#include "taskcmd.h"
#include "cliutils.h"
#include "media.h"

// external modules
#include "adapter/messagesprotocol.h"
#include "service/messages.h"

using namespace klk;
using namespace klk::trans;
using namespace klk::trans::test;

//
// TestTaskCLI class
//

// Constructor
TestTaskCLI::TestTaskCLI() :
    klk::test::TestModuleWithDB("testtranscode_taskcli.sql")
{
}

// Destructor
TestTaskCLI::~TestTaskCLI()
{
}

// The unit test itself
void TestTaskCLI::test()
{
    klk::test::printOut("\nTranscode CLI test (task info) ... ");
    testInvalid();
    testAdd();
    testDel();
}

// Loads all necessary modules at setUp()
void TestTaskCLI::loadModules()
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
void TestTaskCLI::testInvalid()
{
    klk::test::printOut("\n\tInvalid test ...");

    // variables
    adapter::MessagesProtocol proto(klk::test::Factory::instance());
    IMessagePtr out, in;

    // not valid name
    in = m_msgfactory->getMessage(TASKADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params(7);
    params[0] = TASK1_NAME;
    params[1] = INPUTSRC4TASK_NAME;
    params[2] = "invalid";
    params[3] = MEDIA_DEFAULT_SIZE;
    params[4] = MEDIA_LOW_QUALITY;
    params[5] = schedule::ALWAYS;
    params[6] = "0";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // invalid a crontab field
    params.resize(11);
    params[0] = TASK2_NAME;
    params[1] = INPUTSRC4TASK_NAME;
    params[2] = OUTPUTSRC4TASK_NAME2;
    params[3] = MEDIA_DEFAULT_SIZE;
    params[4] = MEDIA_LOW_QUALITY;
    params[5] = "invalid";
    params[6] = "*";
    params[7] = "*";
    params[8] = "*";
    params[9] = "*";
    params[10] = "0";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // invalid duration
    params[0] = TASK2_NAME;
    params[1] = INPUTSRC4TASK_NAME;
    params[2] = OUTPUTSRC4TASK_NAME2;
    params[3] = MEDIA_DEFAULT_SIZE;
    params[4] = MEDIA_LOW_QUALITY;
    params[5] = "*";
    params[6] = "*";
    params[7] = "*";
    params[8] = "*";
    params[9] = "*";
    params[10] = "invalid";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // invalid size
    params[0] = TASK2_NAME;
    params[1] = INPUTSRC4TASK_NAME;
    params[2] = OUTPUTSRC4TASK_NAME2;
    params[3] = "size";
    params[4] = MEDIA_LOW_QUALITY;
    params[5] = "*";
    params[6] = "*";
    params[7] = "*";
    params[8] = "*";
    params[9] = "*";
    params[10] = "0";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // invalid quality
    params[0] = TASK2_NAME;
    params[1] = INPUTSRC4TASK_NAME;
    params[2] = OUTPUTSRC4TASK_NAME2;
    params[3] = MEDIA_DEFAULT_SIZE;
    params[4] = "invalid";
    params[5] = "*";
    params[6] = "*";
    params[7] = "*";
    params[8] = "*";
    params[9] = "*";
    params[10] = "0";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);
}

// Tests task add command
void TestTaskCLI::testAdd()
{
    klk::test::printOut("\n\tAdd tasks test ...");
    // variables
    adapter::MessagesProtocol proto(klk::test::Factory::instance());
    IMessagePtr out, in;

    const std::string TASK1_SCHEDULE = schedule::ALWAYS;
    const std::string TASK2_DURATION = "12345";
    const std::string TASK2_SCHEDULE = "* * * * *";

    testShow(TASK1_NAME, false);
    testShow(TASK1_SCHEDULE, false);
    testShow(TASK2_NAME, false);
    testShow(TASK2_DURATION, false);
    testShow(MEDIA_LOW_QUALITY, false);
    testShow(MEDIA_DEFAULT_SIZE, false);
    testShow(MEDIA_320_240_SIZE, false);

    // valid route name
    in = m_msgfactory->getMessage(TASKADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    // not valid name
    in = m_msgfactory->getMessage(TASKADD_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params(7);
    params[0] = TASK1_NAME;
    params[1] = INPUTSRC4TASK_NAME;
    params[2] = OUTPUTSRC4TASK_NAME1;
    params[3] = MEDIA_DEFAULT_SIZE;
    params[4] = MEDIA_LOW_QUALITY;
    params[5] = TASK1_SCHEDULE;
    params[6] = "0";
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    // test result
    testShow(TASK1_NAME, true);
    testShow(TASK1_SCHEDULE, true);
    testShow(TASK2_NAME, false);
    testShow(TASK2_DURATION, false);
    testShow(TASK2_SCHEDULE, false);
    testShow(MEDIA_LOW_QUALITY, true);
    testShow(MEDIA_DEFAULT_SIZE, true);
    testShow(MEDIA_320_240_SIZE, false);


    // try to add it again (should failed)
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // test result (should not be changed)
    testShow(TASK1_NAME, true);
    testShow(TASK1_SCHEDULE, true);
    testShow(TASK2_NAME, false);
    testShow(TASK2_DURATION, false);
    testShow(TASK2_SCHEDULE, false);

    // valid file name
    params.resize(11);
    params[0] = TASK2_NAME;
    params[1] = INPUTSRC4TASK_NAME;
    params[2] = OUTPUTSRC4TASK_NAME2;
    params[3] = MEDIA_320_240_SIZE;
    params[4] = MEDIA_LOW_QUALITY;
    params[5] = "*";
    params[6] = "*";
    params[7] = "*";
    params[8] = "*";
    params[9] = "*";
    params[10] = TASK2_DURATION;
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    // test result
    testShow(TASK1_NAME, true);
    testShow(TASK1_SCHEDULE, true);
    testShow(TASK2_NAME, true);
    testShow(TASK2_DURATION, true);
    testShow(TASK2_SCHEDULE, true);
    testShow(MEDIA_LOW_QUALITY, true);
    testShow(MEDIA_DEFAULT_SIZE, true);
    testShow(MEDIA_320_240_SIZE, true);
}

// Tests task delete command
void TestTaskCLI::testDel()
{
    klk::test::printOut("\n\tDelete tasks test ...");

    // variables
    adapter::MessagesProtocol proto(klk::test::Factory::instance());
    IMessagePtr out, in;

    testShow(TASK1_NAME, true);
    testShow(TASK2_NAME, true);

    // valid route name
    in = m_msgfactory->getMessage(TASKDEL_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params(1);
    params[0] = TASK1_NAME;
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    // test result
    testShow(TASK1_NAME, false);
    testShow(TASK2_NAME, true);

    // try to del it again (should failed)
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // test result (should not be changed)
    testShow(TASK1_NAME, false);
    testShow(TASK2_NAME, true);

    // valid file name
    params[0] = TASK2_NAME;
    cli::Utils::setProcessParams(in, params);
    out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    // test result
    testShow(TASK1_NAME, false);
    testShow(TASK2_NAME, false);
}

// Test task in the show list
void TestTaskCLI::testShow(const std::string& task, bool presence)
{
    // variables
    adapter::MessagesProtocol proto(klk::test::Factory::instance());
    // test addition of known command
    IMessagePtr in = m_msgfactory->getMessage(TASKSHOW_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params;
    cli::Utils::setProcessParams(in, params);

    IMessagePtr out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    const std::string response = out->getValue(msg::key::CLIRESULT);
    bool found = (response.find(task) != std::string::npos);
    if (presence)
    {
        CPPUNIT_ASSERT(found == true);
    }
    else
    {
        CPPUNIT_ASSERT(found == false);
    }
}
