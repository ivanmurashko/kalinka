/**
   @file testencoder.h
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
   - 2010/03/23 created by ipp (Ivan Murashko)
   - 2010/04/03 TestMain -> TestEncoder by ipp
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTENCODER_H
#define KLK_TESTENCODER_H

#include "testmodule.h"

namespace klk
{
    namespace trans
    {
        /**
           @brief Encoder unit test

           There is a base class to test transcode tasks for different
           @ref  grMedia "media types"

           It tests encoders in different modes
           (with different video quality)

           We create 3 tasks for encoding an avi file into theora

           - The first one with default settings
           - The second one with vsize 320x240 and low video quality
           - The last one with vsize 640x480 and high video quality

           The results are stored at files. The sizes of the files should be different
           The second file should be small, the first - middle size and the last one should be
           max size.

           @ingroup grTransTest
        */
        class TestEncoder : public klk::test::TestModuleWithDB
        {
        public:
            /**
               Constructor

               @param[in] media_type - the media type uuid
            */
            TestEncoder(const std::string& media_type);

            /**
               Destructor
            */
            virtual ~TestEncoder(){}

            /**
               Allocates resources
            */
            virtual void setUp();
        protected:
            /**
               Tests the encoder
               Do the test scenario
            */
            void testDefault();
        private:
            const std::string m_media_type; ///< media type that is tested

            /**
               Loads all necessary modules at setUp()
            */
            virtual void loadModules();
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestEncoder(const TestEncoder& value);

            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestEncoder& operator=(const TestEncoder& value);
        };
    }
}

#endif //KLK_TESTENCODER_H
