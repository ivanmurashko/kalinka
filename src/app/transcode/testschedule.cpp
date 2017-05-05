/**
   @file testschedule.cpp
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
   - 2009/12/30 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "scheduleinfo.h"
#include "testschedule.h"
#include "exception.h"
#include "testutils.h"
#include "defines.h"

using namespace klk;
using namespace klk::trans;

//
// TestScheduleInfo class
//

// Do the test for klk::trans::ScheduleInfo::isMatch
void TestScheduleInfo::testMatch()
{
    klk::test::printOut( "\nTranscode test (crontab parser) ... ");

    CrontabParser always_schedule(schedule::ALWAYS);
    CrontabParser  test1_schedule("15 14 1 * *");
    CrontabParser  test2_schedule("15 14 1 * 2");
    CrontabParser  test3_schedule("10 12 * 1 *");

    CPPUNIT_ASSERT(always_schedule.isMatch());

    struct tm test1;
    memset(&test1, 0, sizeof(test1));
    test1.tm_min = 15;
    test1.tm_hour = 14;
    test1.tm_mday = 1;
    test1.tm_mon = 11;
    test1.tm_wday = 3;
    CPPUNIT_ASSERT(always_schedule.isMatch(&test1) == true);
    CPPUNIT_ASSERT(test1_schedule.isMatch(&test1) == true);
    CPPUNIT_ASSERT(test2_schedule.isMatch(&test1) == false);
    CPPUNIT_ASSERT(test3_schedule.isMatch(&test1) == false);

    struct tm test2;
    memset(&test2, 0, sizeof(test2));
    test2.tm_min = 15;
    test2.tm_hour = 14;
    test2.tm_mday = 1;
    test2.tm_wday = 2;
    CPPUNIT_ASSERT(always_schedule.isMatch(&test2) == true);
    CPPUNIT_ASSERT(test1_schedule.isMatch(&test2) == true);
    CPPUNIT_ASSERT(test2_schedule.isMatch(&test2) == true);
    CPPUNIT_ASSERT(test3_schedule.isMatch(&test2) == false);

    struct tm test3;
    memset(&test3, 0, sizeof(test3));
    test3.tm_min = 10;
    test3.tm_hour = 12;
    test3.tm_mon = 1;
    CPPUNIT_ASSERT(always_schedule.isMatch(&test3) == true);
    CPPUNIT_ASSERT(test1_schedule.isMatch(&test3) == false);
    CPPUNIT_ASSERT(test2_schedule.isMatch(&test3) == false);
    CPPUNIT_ASSERT(test3_schedule.isMatch(&test3) == true);
}
