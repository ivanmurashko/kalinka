/**
   @file testtrans.cpp
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
   - 2009/03/18 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "testtrans.h"
#include "defines.h"
#include "testid.h"
#include "utils.h"
#include "testdefines.h"
#include "testmjpeg.h"
#include "testmpegts.h"
#include "testsourcecli.h"
#include "testtaskcli.h"
#include "testutils.h"
#include "testschedule.h"
#include "testscheduleplay.h"
#include "testarch.h"
#include "testtheora.h"
#include "testflv.h"
#include "traps.h"
#include "testrtp.h"

// modules specific info
#include "service/messages.h"

using namespace klk;
using namespace klk::trans;
using namespace klk::trans::test;

// The function inits module's unit test
void klk_module_test_init()
{
    const std::string TESTMAIN = MODNAME + "/main";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestTranscode,
                                          TESTMAIN);
    CPPUNIT_REGISTRY_ADD(TESTMAIN, MODNAME);

    const std::string TESTFLV = MODNAME + "/flv";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestFLV,
                                          TESTFLV);
    CPPUNIT_REGISTRY_ADD(TESTFLV, MODNAME);

    const std::string TESTTHEORA = MODNAME + "/theora";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestTheora,
                                          TESTTHEORA);
    CPPUNIT_REGISTRY_ADD(TESTTHEORA, MODNAME);


    const std::string TESTSCHEDULE = MODNAME + "/schedule";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestSchedulePlay,
                                          TESTSCHEDULE);
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestScheduleInfo,
                                          TESTSCHEDULE);
    CPPUNIT_REGISTRY_ADD(TESTSCHEDULE, MODNAME);

    const std::string TESTMJPEG = MODNAME + "/mjpeg";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestMJpeg,
                                          TESTMJPEG);
    CPPUNIT_REGISTRY_ADD(TESTMJPEG, MODNAME);

    const std::string TESTMPEGTS = MODNAME + "/mpegts";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestMpegts,
                                          TESTMPEGTS);
    CPPUNIT_REGISTRY_ADD(TESTMPEGTS, MODNAME);


    const std::string TESTARCH = MODNAME + "/arch";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestArch,
                                          TESTARCH);
    CPPUNIT_REGISTRY_ADD(TESTARCH, MODNAME);

    const std::string TESTCLI = MODNAME + "/cli";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestTaskCLI,
                                          TESTCLI);
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestSourceCLI,
                                          TESTCLI);
    CPPUNIT_REGISTRY_ADD(TESTCLI, MODNAME);

    const std::string TESTRTP = MODNAME + "/rtp";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestRTP,
                                          TESTRTP);
    CPPUNIT_REGISTRY_ADD(TESTRTP, MODNAME);

    CPPUNIT_REGISTRY_ADD(MODNAME, klk::test::ALL);
}

//
// TestTranscode class
//

// Constructor
TestTranscode::TestTranscode() :
    TestBase("testtranscode.sql")
{
}

// Test constructor
void TestTranscode::setUp()
{
    // remove the result file
    base::Utils::unlink(OUTPUTFILE);
    base::Utils::unlink(OUTPUTROUTE);

    TestBase::setUp();

    // wait for awhile before startup
    // this guarantee that the destination
    // network sink will not exist during pipeline startup
    sleep(SCHEDULE_INTERVAL*2);
}

// The unit test that waits for EOS for the
// transcoded file
void TestTranscode::testFull()
{
    klk::test::printOut( "\nTranscode test (full) ... ");

    m_scheduler.start();

    // wait for awhile
    sleep(60 + SCHEDULE_INTERVAL);

    // stop all others
    m_scheduler.stop();

    // check result in the started thread
    m_scheduler.checkResult();

    // check running count
    // all should be in EOS state
    TaskInfoList tasks = getTaskList();
    CPPUNIT_ASSERT(tasks.size() == 2);
    for (TaskInfoList::iterator task = tasks.begin(); task != tasks.end(); task++)
    {
        // currently the task has to be finished
        CPPUNIT_ASSERT((*task)->getActualDuration() == 0);
        // task should be run 1 time
        CPPUNIT_ASSERT((*task)->getRunningCount() == 1);
        // Note: total avi file duration is 33 seconds
        CPPUNIT_ASSERT((*task)->getRunningTime() >= 30 * 1000000000ULL);
    }

    // check created files
    BinaryData input =
        base::Utils::readWholeDataFromFile(INPUTFILE);
    CPPUNIT_ASSERT(input.empty() == false);
    BinaryData output =
        base::Utils::readWholeDataFromFile(OUTPUTFILE);
    CPPUNIT_ASSERT(output.empty() == false);

    CPPUNIT_ASSERT(input == output);

    // test route
    BinaryData route =
        base::Utils::readWholeDataFromFile(OUTPUTROUTE);
    CPPUNIT_ASSERT(route.empty() == false);

    // test traps as final part
    testTrap();
}

// The test breaks the transcoding process in
// the middle of transcoding
void TestTranscode::testShort()
{
    klk::test::printOut("\nTranscode test (short) ... ");

    m_scheduler.start();

    // wait for awhile
    sleep(10 + SCHEDULE_INTERVAL);

    // check running count
    // all should be in running state
    TaskInfoList tasks = getTaskList();
    CPPUNIT_ASSERT(tasks.size() == 2);
    for (TaskInfoList::iterator task = tasks.begin(); task != tasks.end(); task++)
    {
        // all task should be in running state
        CPPUNIT_ASSERT((*task)->getActualDuration() > 0);
        CPPUNIT_ASSERT((*task)->getRunningCount() == 0);
    }

    // stop all others
    m_scheduler.stop();

    // check result in the started thread
    m_scheduler.checkResult();

    // check created files
    BinaryData input =
        base::Utils::readWholeDataFromFile(INPUTFILE);
    CPPUNIT_ASSERT(input.empty() == false);
    BinaryData output =
        base::Utils::readWholeDataFromFile(OUTPUTFILE);
    CPPUNIT_ASSERT(output.empty() == false);

    CPPUNIT_ASSERT(input.size() > output.size());
    // compare data
    CPPUNIT_ASSERT(memcmp(input.toVoid(), output.toVoid(), output.size()) == 0);

    // test route
    BinaryData route =
        base::Utils::readWholeDataFromFile(OUTPUTROUTE);
    CPPUNIT_ASSERT(route.empty() == false);

    // test traps as final part
    testTrap();
}

// Do SNMP traps test
void TestTranscode::testTrap()
{
    klk::test::TrapInfoVector traps = getTraps();
    // there are should be traps here
    CPPUNIT_ASSERT(traps.empty() == false);
    // each task should be in traps
    u_int task1_count = 0, task2_count = 0;
    // all traps should be one one type
    for (klk::test::TrapInfoVector::iterator i = traps.begin();
         i != traps.end(); i++)
    {
        CPPUNIT_ASSERT(klk::test::IsOIDMatch()(*i, trap::TASKFAILED) == true);
        if (i->getValue() == test::TASK1_UUID)
        {
            task1_count++;
        }
        else if (i->getValue() == test::TASK2_UUID)
        {
            task2_count++;
        }
        else
        {
            // should not be here
            // only task1 of task2 are possible
            CPPUNIT_ASSERT(0);
        }
    }

    CPPUNIT_ASSERT(task1_count == task2_count);
    CPPUNIT_ASSERT(task1_count > 0);
}
