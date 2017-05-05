/**
   @file testbase.h
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
   - 2010/01/03 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTBASE_H
#define KLK_TESTBASE_H

#include "testmodule.h"
#include "testthread.h"
#include "transinfo.h"

namespace klk
{
    namespace trans
    {

        /** @defgroup grTransTest Transcode application utest

            @ingroup grTrans

            @{
        */

        /**
           @brief Transcoder unit test

           The Transcoder unit test
        */
        class TestBase : public test::TestModuleWithDB
        {
        public:
            /**
               Constructor

               @param[in] sql - the sql script name (the script should be executed)
            */
            explicit TestBase(const std::string& sql);

            /**
               Destructor
            */
            virtual ~TestBase(){}

            /// The unit test data preparation
            virtual void setUp();

            /// The unit test data cleanup
            virtual void tearDown();
        protected:
            test::Scheduler m_scheduler; ///< the scheduler

            /**
               Retrives tasks list

               @return the list
            */
            const TaskInfoList getTaskList();
        private:
            /**
               Loads all necessary modules at setUp()
            */
            virtual void loadModules();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestBase(const TestBase& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestBase& operator=(const TestBase& value);
        };

        /** @} */

    }
}

#endif //KLK_TESTBASE_H
