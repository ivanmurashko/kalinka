/**
   @file cliapptest.h
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
   - 2009/02/09 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_CLIAPPTEST_H
#define KLK_CLIAPPTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include <string>
#include <map>

#include "ifactory.h"
#include "dbscriptlauncher.h"
#include "db.h"

namespace klk
{
    namespace test
    {
        /**
           @brief CLI commands for Applications test

           The class tests CLI commands common for all applications

           @ingroup grTest
        */
        class CLIAppTest : public CppUnit::TestFixture
        {
            CPPUNIT_TEST_SUITE(CLIAppTest);
            CPPUNIT_TEST(testProcess);
            CPPUNIT_TEST(testComplete);
            CPPUNIT_TEST_SUITE_END();
        public:
            /**
               Constructor
            */
            void setUp();

            /**
               Destructor
            */
            void tearDown();

            /**
               The unit test itself for command processing
            */
            void testProcess();

            /**
               The unit test itself for command completions
            */
            void testComplete();
        private:
            DBScriptLauncher* m_launcher; ///< db script launcher
            IFactory* m_factory; ///< factory
            db::DB* m_db; ///< the db

            /**
               Internal map with app info
            */
            typedef std::map<std::string, bool> StringBoolMap;

            /**
               Retrives application list with enable/disable info

               @return the current applications
            */
            const StringBoolMap getApps();
        };
    }
}

#endif //KLK_CLIAPPTEST_H