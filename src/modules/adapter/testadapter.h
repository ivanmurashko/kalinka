/**
   @file testadapter.h
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
   - 2008/11/10 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTADAPTER_H
#define KLK_TESTADAPTER_H

#include "testmodule.h"

namespace klk
{
    namespace adapter
    {
        /**
           @brief Adapter unit test

           The Adapter unit test

           @ingroup grTest
        */
        class TestAdapter : public test::TestModuleWithDB
        {
            CPPUNIT_TEST_SUITE(TestAdapter);
            CPPUNIT_TEST(testStartStop);
            CPPUNIT_TEST(test1);
            CPPUNIT_TEST(test2);
            CPPUNIT_TEST_SUITE_END();
        public:
            /**
               Constructor
            */
            TestAdapter();

            /**
               Destructor
            */
            virtual ~TestAdapter(){}

            /**
               The unit test for startup/stop

               It tests values at the db
            */
            void testStartStop();

            /**
               The unit test first start
            */
            void test1();

            /**
               The unit test second start
            */
            void test2();
        private:
            /**
               The unit test itself
            */
            void test();

            /**
               Loads all necessary modules at setUp()
            */
            virtual void loadModules();

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestAdapter(const TestAdapter& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestAdapter& operator=(const TestAdapter& value);

        };
    };
};


#endif //KLK_TESTADAPTER_H
