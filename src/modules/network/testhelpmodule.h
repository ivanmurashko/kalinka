/**
   @file testhelpmodule.h
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
   - 2008/11/07 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTHELPMODULE_H
#define KLK_TESTHELPMODULE_H

#include "helpmodule.h"

namespace klk
{
    namespace net
    {
        /**
           @brief Test helpmodule for messaging core tests

           Test helpmodule for messaging core tests

           @ingroup grNetModule
        */
        class TestHelpModule : public klk::test::HelpModule
        {
        public:
            /**
               Constructor
            */
            TestHelpModule(IFactory *factory);

            /**
               Destructor
            */
            virtual ~TestHelpModule(){}
        private:
            /**
               @copydoc klk::test::HelpModule::doTest
            */
            virtual void doTest();

            /**
               Do the main test
            */
            void doMainTest();

            /**
               Do the test

               Main test with ok requests
            */
            void doTest1(IMessageFactory* msgfactory, const IMessagePtr& request);

            /**
               Do the test

               Test with wrong requests
            */
            void doTest2(IMessageFactory* msgfactory, const IMessagePtr& request);

            /**
               Do the test

               Test with request for routes from external mediaservers
            */
            void doTest3(IMessageFactory* msgfactory, const IMessagePtr& request);

            /**
               Do the cleanup test
            */
            void doTestCleanup();

            /**
               Retrives lock state for specified route

               @param[in] route - the route to be checked

               @return
               - 0 the resource is not locked
               - 1 the resource is locked
               - -1 there was an error
            */
            int getLockState(const std::string& route);
        private:
            /**
               Copy constructor
            */
            TestHelpModule(const TestHelpModule&);

            /**
               Assignment opearator
            */
            TestHelpModule& operator=(const TestHelpModule&);

        };
    }
}

#endif //KLK_TESTHELPMODULE_H
