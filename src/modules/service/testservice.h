/**
   @file testservice.h
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
   - 2008/11/14 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTSERVICE_H
#define KLK_TESTSERVICE_H

#include "testmodule.h"

namespace klk
{
    namespace srv
    {
        /**
           @brief Service unit test

           The Service unit test

           @ingroup grSysModuleTest
        */
        class TestService : public test::TestModuleWithDB
        {
            CPPUNIT_TEST_SUITE(TestService);
            CPPUNIT_TEST(testApps);
            CPPUNIT_TEST_SUITE_END();
        public:
            /**
               Constructor
            */
            TestService();

            /**
               Destructor
            */
            virtual ~TestService(){}

            /**
               The unit test for Application load/unload
            */
            void testApps();
        private:
            /**
               Loads all necessary modules at setUp()
            */
            virtual void loadModules();

            /**
               Retrives load count for the application with name specified as argument

               @param[in] appname the application name

               @return how many times the applicatiopn was run
            */
            int getRunCount(const std::string& appname);

            /**
               Tests is the application was started or not

               @param[in] id - the application module id

               @return
               - true the application was started
               - false - the application was not started
            */
            static bool isStarted(const std::string& id);

            /**
               Sends SIGSEGV to the application with id specified at
               the argument

               @param[in] app_id - the application id
            */
            void sendSigSegv(const std::string& app_id);
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestService(const TestService& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestService& operator=(const TestService& value);
        };
    }
}


#endif //KLK_TESTSERVICE_H
