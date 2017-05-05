/**
   @file teststreamer.h
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
   - 2009/02/19 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTSTREAMER_H
#define KLK_TESTSTREAMER_H

#include "testmodule.h"
#include "testthread.h"

namespace klk
{
    namespace http
    {
        /** @defgroup grTestHTTP HTTP streamer application unit tests
            @{

            There are several unit tests for HTTP streamer application

            @ingroup grHTTP
        */

        /**
           @brief Streamer unit test

           The Streamer unit test

           The test starts 2 producer threads that provides
           simple text data for HTTP streamer. The data
           assigned to two different paths.

           There are also several consumer threads that tests the
           data on different path by means of HTTP GET and HEAD requests
        */
        class TestStreamer : public test::TestModuleWithDB
        {
        public:
            /**
               Constructor

               @param[in] sqlscript - the sql db creation script
            */
            TestStreamer(const std::string& sqlscript);

            /// Destructor
            virtual ~TestStreamer(){}

            /// Prepares the test data
            virtual void setUp();

            /// Clears the test data
            virtual void tearDown();
        protected:
            test::Scheduler m_scheduler; ///< the scheduler
            test::ThreadList m_consumers; ///< consumers threads
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
            TestStreamer(const TestStreamer& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestStreamer& operator=(const TestStreamer& value);
        };

        /** @} */

    }
}
#endif //KLK_TESTSTREAMER_H
