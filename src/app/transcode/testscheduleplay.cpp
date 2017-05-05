/**
   @file testscheduleplay.cpp
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
   - 2010/01/03 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testscheduleplay.h"
#include "defines.h"
#include "utils.h"
#include "testdefines.h"
#include "testutils.h"

using namespace klk;
using namespace klk::trans;

//
// TestSchedulePlay class
//

// Constructor
TestSchedulePlay::TestSchedulePlay() :
    TestBase("testtranscode_scheduleplayback.sql")
{
}

// Test constructor
void TestSchedulePlay::setUp()
{
    // remove the result file
    base::Utils::unlink(trans::test::OUTPUTROUTE);
    TestBase::setUp();
}

// The unit test that waits for EOS for the
// transcoded file and repeate it again
void TestSchedulePlay::testAlways()
{
    klk::test::printOut( "\nTranscode test (schedule playback) ... ");

    m_scheduler.start();

    // wait for awhile
    u_int count = 4;

    /*
      We can fail one time at startup and it can be not finished
      Thus
      getRunningCount() == count
      or
      getRunningCount() == count -1
      or
      getRunningCount() == count -2
    */
    time_t duration = SCHEDULE_INTERVAL * (count + 2);

    sleep(duration);

    // stop all others
    m_scheduler.stop();

    // check result in the started thread
    m_scheduler.checkResult();

    // check running count
    // all should be in EOS state
    TaskInfoList tasks = getTaskList();
    CPPUNIT_ASSERT(tasks.size() == 1);

    for (TaskInfoList::iterator task = tasks.begin(); task != tasks.end(); task++)
    {
        CPPUNIT_ASSERT((*task)->getRunningCount() >= count);
    }

    // test route
    BinaryData route =
        base::Utils::readWholeDataFromFile(trans::test::OUTPUTROUTE);
    CPPUNIT_ASSERT(route.empty() == false);
}
