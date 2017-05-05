/**
   @file testplugin.h
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
   - 2009/10/03 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTPLUGIN_H
#define KLK_TESTPLUGIN_H

#include <cppunit/extensions/HelperMacros.h>

#include "dvb/utils/testhelper.h"
#include "iscanprocessor.h"

namespace klk
{
    namespace dvb
    {
        namespace scan
        {
            /**
               The gstreamer module test

               @ingroup grTest
            */
            class TestPlugin : public CppUnit::TestFixture, private TestHelper
            {
                CPPUNIT_TEST_SUITE(TestPlugin);
                CPPUNIT_TEST_EXCEPTION(testWrong, klk::Exception);
                CPPUNIT_TEST(testOk);
                CPPUNIT_TEST_SUITE_END();
            public:
                /**
                   Constructor
                */
                TestPlugin();

                /**
                   Destructor
                */
                virtual ~TestPlugin(){}

                /**
                   The unit test for OK requests
                */
                void testOk();

                /**
                   The unit test for wrong requests
                */
                void testWrong();

                /**
                   Set up context before running a test.
                */
                virtual void setUp();

                /**
                   Clean up after the test run.
                */
                virtual void tearDown();
            private:
                IProcessorPtr m_processor; ///< processor for tests
                u_int m_sdt_count; ///< SDT count
                u_int m_pmt_count; ///< PMT count

                /**
                   Save SDT data

                   @param[in] no - the channel number
                   @param[in] name - the channel name
                   @param[in] prvname - the provider name
                   @param[in] scrambled - is the channel scrambled or not
                */
                void saveSDT(u_int no, const std::string& name,
                             const std::string& prvname,
                             bool scrambled);

                /**
                   Save PMT data

                   @param[in] no - the channel number
                   @param[in] pid - the pid
                   @param[in] type - the pid's type
                */
                void savePMT(u_int no, u_int pid, const std::string& type);
            private:
                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                TestPlugin(const TestPlugin& value);

                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                TestPlugin& operator=(const TestPlugin& value);
            };
        }
    }
}

#endif //KLK_TESTPLUGIN_H
