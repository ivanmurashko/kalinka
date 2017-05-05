/**
   @file testflv.h
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
   - 2010/03/23 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTFLV_H
#define KLK_TESTFLV_H

#include "testencoder.h"

namespace klk
{
    namespace trans
    {
        /**
           @brief FLV unit test

           It tests flash encoder in different modes
           (with different video quality)

           @see klk::trans::TestEncoder for the test scenario description

           @ingroup grTransTest
        */
        class TestFLV : public TestEncoder
        {
            CPPUNIT_TEST_SUITE(TestFLV);
            CPPUNIT_TEST(test);
            CPPUNIT_TEST_SUITE_END();
        public:
            /**
               Constructor
            */
            TestFLV();

            /**
               Destructor
            */
            virtual ~TestFLV(){}

            /**
               Tests the encoder with default settings
            */
            void test();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestFLV(const TestFLV& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestFLV& operator=(const TestFLV& value);
        };
    }
}

#endif //KLK_TESTFLV_H
