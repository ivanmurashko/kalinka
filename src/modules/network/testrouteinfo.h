/**
   @file testrouteinfo.h
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
   - 2009/07/02 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTROUTEINFO_H
#define KLK_TESTROUTEINFO_H

#include <cppunit/extensions/HelperMacros.h>

#include "iresources.h"

namespace klk
{
    namespace net
    {
        /**
           @brief Network unit test

           The Network unit test

           @ingroup grTest
        */
        class TestRouteInfo : public CppUnit::TestFixture
        {
            CPPUNIT_TEST_SUITE(TestRouteInfo);
            CPPUNIT_TEST(test);
            CPPUNIT_TEST_SUITE_END();
        public:
            /**
               Constructor
            */
            TestRouteInfo();

            /**
               Destructor
            */
            virtual ~TestRouteInfo(){}

            /**
               Fills initial data for utest
            */
            virtual void setUp();

            /**
               Clears utest data
            */
            virtual void tearDown();

            /**
               The unit test for route info
            */
            void test();
        private:
            IDevPtr m_dev; ///< test dev

            /**
               Creates devs
            */
            void createDev();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestRouteInfo(const TestRouteInfo& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestRouteInfo& operator=(const TestRouteInfo& value);
        };
    }
}

#endif //KLK_TESTROUTEINFO_H
