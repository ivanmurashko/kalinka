/**
   @file testscheduleplay.h
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

#ifndef KLK_TESTSCHEDULEPLAY_H
#define KLK_TESTSCHEDULEPLAY_H

#include "testbase.h"

namespace klk
{
    namespace trans
    {
        /**
           @brief Scheduled playback unit test

            There is an utest for @ref grTrans "transcode application".
            The test gives data from a file source and sends it to a network route

            The file has 33 seconds duration and during the test it has to be played
            more than 2 times

           @ingroup grTransTest
        */
        class TestSchedulePlay : public TestBase
        {
            CPPUNIT_TEST_SUITE(TestSchedulePlay);
            CPPUNIT_TEST(testAlways);
            CPPUNIT_TEST_SUITE_END();
        public:
            /**
               Constructor
            */
            TestSchedulePlay();

            /**
               Destructor
            */
            virtual ~TestSchedulePlay(){}

            /**
               The utest for always playing
            */
            void testAlways();

            /**
               Allocates resources
            */
            virtual void setUp();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestSchedulePlay(const TestSchedulePlay& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestSchedulePlay& operator=(const TestSchedulePlay& value);
        };
    }
}
#endif //KLK_TESTSCHEDULEPLAY_H
