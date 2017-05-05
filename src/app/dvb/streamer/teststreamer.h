/**
   @file teststreamer.h
   @brief This file is part of Kalinka mediaserver.
   @author ipp <ivan.murashko@gmail.com>

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
   - 2008/11/03 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#include <cppunit/extensions/HelperMacros.h>

#ifndef KLK_TESTSTREAMER_H
#define KLK_TESTSTREAMER_H

#include "testbase.h"
#include "stringlist.h"

namespace klk
{
    namespace dvb
    {
        namespace stream
        {
            /** @defgroup grDVBStreamerTest DVB streamer utests

                The group keeps defenitions for DVB streamer unit tests

                @ingroup grDVBStreamer

                @{
            */

            /**
               @brief Streamer unit test

               The base DVB streamer unit tests

               It do the following tests
               Add several channels to stream and see that all of them are streaming.
               One of the channel can not be streamed (no resource for them).
               The situation is also checked.

               Delete a channel and see that the deleted channel is not stream.

               Add the deleted channel back and see that there is a stream of the channel.

               Emulates the streamer restart.

               Emulates failures and reaction on the failures.

               The Streamer unit test
            */
            class TestStreamer : public TestBase
            {
                CPPUNIT_TEST_SUITE(TestStreamer);
                CPPUNIT_TEST(testMain);
                CPPUNIT_TEST(testFailures);
                CPPUNIT_TEST_SUITE_END();
            public:
                /**
                   Constructor
                */
                TestStreamer();

                /**
                   Destructor
                */
                virtual ~TestStreamer(){}

                /**
                   The test itself
                */
                void testMain();

                /**
                   The test checks failures stations processing
                */
                void testFailures();
            private:
                /**
                   First test after startup
                */
                void testFirst();

                /**
                   Test add/remove channels
                */
                void testAddRemove();

                /**
                   Test restart
                */
                void testRestart();

                /**
                   Base test for failures
                */
                void testFailuresBase();

                /**
                   Promblems tests

                   Signal lost reaction and restore
                */
                void testProblems();

                /**
                   Test channels
                */
                void testChannels();
            private:
                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                TestStreamer(const TestStreamer& value);

                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                TestStreamer& operator=(const TestStreamer& value);
            };

            /** @} */

        }
    }
}
#endif //KLK_TESTSTREAMER_H
