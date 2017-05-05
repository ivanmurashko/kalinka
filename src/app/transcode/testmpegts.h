/**
   @file testmpegts.h
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
   - 2010/05/18 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTMPEGTS_H
#define KLK_TESTMPEGTS_H

#include "testmodule.h"

namespace klk
{
    namespace trans
    {

        /**
           @brief Transcoder unit test for mpegts source

           There is a test for ticket #213. It tests mpegts (DVB)
           source

           @ingroup grTransTest

           @note FIXME!!! code duplicate with TestMjpeg
        */
        class TestMpegts : public klk::test::TestModuleWithDB
        {
            CPPUNIT_TEST_SUITE(TestMpegts);
            CPPUNIT_TEST(testFLV);
            CPPUNIT_TEST(testTheora);
            CPPUNIT_TEST_SUITE_END();
        public:
            /**
               Constructor
            */
            TestMpegts();

            /**
               Destructor
            */
            virtual ~TestMpegts(){}

            /**
               Do the test for klk::media::FLV as destination format

               @note See ticket #185 for more info about the test
            */
            void testFLV();

            /**
               Do the test for klk::media::THEORA as destination format

               @note See ticket #210 for more info about the test
            */
            void testTheora();

            /**
               Allocates resources
            */
            virtual void setUp();
        private:
            /**
               Loads all necessary modules at setUp()
            */
            virtual void loadModules();

            /**
               Do the test (common operations)

               @param[in] media_type - the output media type to be tested
            */
            void test(const std::string& media_type);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestMpegts(const TestMpegts& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestMpegts& operator=(const TestMpegts& value);
        };
    }
}


#endif //KLK_TESTMPEGTS_H
