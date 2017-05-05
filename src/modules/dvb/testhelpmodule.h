/**
   @file testhelpmodule.h
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
   - 2008/11/07 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTHELPMODULE_H
#define KLK_TESTHELPMODULE_H

#include <list>

#include "module.h"
#include "thread.h"
#include "xml.h"
#include "iresources.h"
#include "testmodule.h"
#include "testdefines.h"
#include "maintestdefines.h"

namespace klk
{
    namespace dvb
    {
        /**
           @brief Test helpmodule id

           test helpmodule id

           @ingroup grTest
        */
        const std::string TEST_MOD_ID = test::HELPPREFIX + "dvb";


        /**
           The test message to start streaming test for DVB

           @ingroup grTest
        */
        const std::string TEST_STREAM_MSG_ID = "test_stream_message_dvb";

        /**
           The test message to start scaning test for DVB

           @ingroup grTest
        */
        const std::string TEST_SCAN_MSG_ID = "test_scan_start_message_dvb";

        /**
           @brief Test helpmodule for messaging core tests

           Test helpmodule for messaging core tests

           @ingroup grTest
        */
        class TestHelpModule : public klk::Module
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

            /**
               Gets the result
            */
            klk::Result getResult() {return m_rc;}
        private:
            klk::Result m_rc; ///< result

            /**
               Gets a human readable helpmodule name

               @return the name
            */
            virtual const std::string getName() const throw()
            {return "dvbtest";}

            /**
               Process an async message (streaming)

               @param[in] msg the message to be processed

               @exception @ref klk::Exception
            */
            void doStream(const IMessagePtr& msg);

            /**
               Process an async message (scanning)

               @param[in] msg the message to be processed

               @exception @ref klk::Exception
            */
            void doScan(const IMessagePtr& msg);

            /// @copydoc klk::IModule::registerProcessors
            virtual void registerProcessors();

            /// @copydoc klk::IModule::getType
            virtual mod::Type getType() const throw() {return mod::MOD;}
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
