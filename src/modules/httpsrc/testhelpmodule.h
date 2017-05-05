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
   - 2009/12/06 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTHELPMODULE_H
#define KLK_TESTHELPMODULE_H

#include "helpmodule.h"

namespace klk
{
    namespace httpsrc
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
               Tests invalid (not existent id)

               @exception klk::Exception
            */
            void testInvalid();

            /**
               Tests first camera

               @exception klk::Exception
            */
            void testFirst();

            /**
               Tests second camera

               @exception klk::Exception
            */
            void testSecond();
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
