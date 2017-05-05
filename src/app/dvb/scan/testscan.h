/**
   @file testscan.h
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
   - 2009/06/14 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/


#ifndef KLK_TESTSCAN_H
#define KLK_TESTSCAN_H

#include <cppunit/extensions/HelperMacros.h>

#include "testmodule.h"
#include "testprocessor.h"

namespace klk
{
    namespace dvb
    {
        namespace scan
        {
            /**
               @brief DVB scan unit test

               The DVB scan unit test

               @ingroup grTest
            */
            class TestScan : public test::TestModuleWithDB
            {
                CPPUNIT_TEST_SUITE(TestScan);
                CPPUNIT_TEST(testDVBT);
                CPPUNIT_TEST(testDVBS);
                CPPUNIT_TEST_SUITE_END();
            public:
                /**
                   Constructor
                */
                TestScan();

                /**
                   Destructor
                */
                virtual ~TestScan(){}

                /**
                   The unit test itself for DVB-T cards
                */
                void testDVBT();

                /**
                   The unit test itself for DVB-S cards
                */
                void testDVBS();
            private:
                TestProcessorPtr m_processor; ///< test processor smart pointer

                /**
                   Loads all necessary modules at setUp()
                */
                virtual void loadModules();

                /**
                   Creates devs
                */
                void createDev();

                /**
                   Tests scan procedure for DVB-S

                   @param[in] first - is there first call or not
                */
                void testScanS(bool first);

                /**
                   Tests scan procedure for DVB-T

                   @param[in] first - is there first call or not
                */
                void testScanT(bool first);

                /**
                   Tests pids
                */
                void testPids(const std::string& channel);
            private:
                /**
                   Copy constructor
                   @param[in] value - the copy param
                */
                TestScan(const TestScan& value);

                /**
                   Assigment operator
                   @param[in] value - the copy param
                */
                TestScan& operator=(const TestScan& value);
            };
        }
    }
}

#endif //KLK_TESTSCAN_H
