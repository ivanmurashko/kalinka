/**
   @file restest.h
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
   - 2008/09/13 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_RESTEST_H
#define KLK_RESTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include "resources.h"
#include "dbscriptlauncher.h"
#include "ifactory.h"
#include "basedev.h"

namespace klk
{
    namespace dev
    {
        /**
           @brief Test dev

           Test dev

           @ingroup grTest
        */
        class TestDev : public Base
        {
        public:
            /**
               Constructor

               @param[in] factory - the factory
            */
            TestDev(IFactory* factory);

            /**
               Destructor
            */
            virtual ~TestDev(){}

            /**
               @copydoc IDev::update()
            */
            virtual void update();

            /**
               @copydoc IDev::updateDB()
            */
            virtual void updateDB();

            /**
               @copydoc IDev::equal_to()
            */
            virtual bool equal_to(const IDevPtr& dev) const;
        private:
            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestDev& operator=(const TestDev& value);

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestDev(const TestDev& value);
        };

        /**
           @brief Test resources

           Test resources

           @ingroup grTest
        */
        class TestResources : public BaseResources
        {
        public:
            /**
               Constructor

               @param[in] type - the base dev type
            */
            TestResources(const std::string& type);

            /**
               Destructor
            */
            virtual ~TestResources(){}

            /**
               Checks main devices

               That are used by all modules

               @exception @ref klk::Exception
            */
            virtual void initDevs(){}
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestResources(const TestResources& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestResources& operator=(const TestResources& value);
        };

        /**
           @brief The resources unit test

           ResourcesTest

           @ingroup grTest
        */
        class ResourcesTest : public CppUnit::TestFixture
        {
            CPPUNIT_TEST_SUITE(ResourcesTest);
            CPPUNIT_TEST(test);
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
               Tests resources
            */
            void test();
        private:
            IFactory *m_factory; ///< factory
            test::DBScriptLauncher* m_launcher; ///< db script launcher
        };
    }
}

#endif //KLK_RESTEST_H
