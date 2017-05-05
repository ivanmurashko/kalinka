/**
   @file testcli.h
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
   - 2009/03/04 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTCLI_H
#define KLK_TESTCLI_H

#include "testmodule.h"

namespace klk
{
    namespace net
    {
        /**
           @brief Network unit test

           The Network unit test

           @ingroup grTest
        */
        class TestCLI : public test::TestModuleWithDB
        {
            CPPUNIT_TEST_SUITE(TestCLI);
            CPPUNIT_TEST(testMain);
            CPPUNIT_TEST_SUITE_END();
        public:
            /**
               Constructor
            */
            TestCLI();

            /**
               Destructor
            */
            virtual ~TestCLI(){}

            /**
               The unit test for main functionality
            */
            void testMain();
        private:
            /**
               Loads all necessary modules at setUp()
            */
            virtual void loadModules();

            /**
               Test that response have the specified route

               @param[in] route - the IP address to be checked
               @param[in] presence - should the route be at the list or not
            */
            void testList(const std::string& route, bool presence);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestCLI(const TestCLI& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestCLI& operator=(const TestCLI& value);
        };
    }
}

#endif //KLK_TESTCLI_H
