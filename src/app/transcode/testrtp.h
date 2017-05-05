/**
   @file testrtp.h
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
   - 2011/02/13 created by ipp (Ivan Murashko)
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTRTP_H
#define KLK_TESTRTP_H

#include "testmodule.h"

namespace klk
{
    namespace trans
    {
        /**
           @brief RTP/MpegTS unit test

           The test starts RTP with mpegts as payload and checks that all data
           sent via RTP were sucessfully processed (saved at a file
        */
        class TestRTP : public klk::test::TestModuleWithDB
        {
            CPPUNIT_TEST_SUITE(TestRTP);
            CPPUNIT_TEST(test);
            CPPUNIT_TEST_SUITE_END();
        public:
            /// Constructor
            TestRTP();

            /// Destructor
            virtual ~TestRTP(){}

            /// The test itself
            void test();

            /// The unit test data preparation
            virtual void setUp();
        private:
            /// @copydoc klk::test::TestModule::loadModule
            virtual void loadModules();
        private:
            /// Fake copy constructor
            TestRTP(const TestRTP&);

            /// Fake assigment operator
            TestRTP& operator=(const TestRTP&);
        };
    }
}

#endif //KLK_TESTRTP_H
