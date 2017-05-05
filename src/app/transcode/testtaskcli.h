/**
   @file testtaskcli.h
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
   - 2009/11/30 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTTASKCLI_H
#define KLK_TESTTASKCLI_H

#include "testmodule.h"

namespace klk
{
    namespace trans
    {
        /**
           @brief The task related CLI commands utest

           The class tests CLI commands for add/delete/show transcode tasks

           @ingroup grTransTest
        */
        class TestTaskCLI : public klk::test::TestModuleWithDB
        {
            CPPUNIT_TEST_SUITE(TestTaskCLI);
            CPPUNIT_TEST(test);
            CPPUNIT_TEST_SUITE_END();
        public:
            /**
               Constructor
            */
            TestTaskCLI();

            /**
               Destructor
            */
            virtual ~TestTaskCLI();

            /**
               The unit test itself
            */
            void test();
        private:
            /**
               Loads all necessary modules at setUp()
            */
            virtual void loadModules();

            /**
               Tests invalid parameters
            */
            void testInvalid();

            /**
               Tests task add command
            */
            void testAdd();

            /**
               Tests task delete command
            */
            void testDel();

            /**
               Test task in the show list

               @param[in] task - the task's name to be checked
               @param[in] presence - is the task present on the check list or not
            */
            void testShow(const std::string& task, bool presence);
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestTaskCLI& operator=(const TestTaskCLI& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestTaskCLI(const TestTaskCLI& value);
        };
    }
}

#endif //KLK_TESTTASKCLI_H
