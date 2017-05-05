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
   - 2008/10/18 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTHELPMODULE_H
#define KLK_TESTHELPMODULE_H

#include "module.h"
#include "thread.h"
#include "testmodule.h"
#include "maintestdefines.h"

namespace klk
{
    namespace msgcore
    {
        /** @addtogroup grTestMsgCore
            @{
        */


        /**
           @brief Test module id

           test module id
        */
        const std::string TEST_MOD4ASYNC_ID =
            klk::test::HELPPREFIX + "async";

        /**
           @brief Test module id

           test module id
        */
        const std::string TEST_MOD4SYNC_ID =
            klk::test::HELPPREFIX + "sync";


        /**
           @brief Test message response data

           test message response
        */
        const std::string TEST_MSG_DATA = "test message data";

        /**
           @brief Test module for messaging core tests

           Test module for messaging core tests
        */
        class TestModule4ASync : public klk::Module
        {
        public:
            /**
               Constructor
            */
            TestModule4ASync(IFactory *factory);

            /**
               Destructor
            */
            virtual ~TestModule4ASync(){}

            /**
               gets count (how many times the request message was got)

               @return the count
            */
            u_int getCount(){return m_count;}
        private:
            u_int m_count; ///< the count

            /**
               Gets a human readable module name

               @return the name
            */
            virtual const std::string getName() const throw()
            {return "msgcoretest4async";}

            /**
               Process an async message

               @param[in] msg the message to be processed

               @exception klk::Exception
            */
            void processASync(const IMessagePtr& msg);

            /**
               Process a sync message

               @param[in] in - the input message
               @param[out] out - the output message

               @exception klk::Exception
            */
            void processSync(const IMessagePtr& in,
                             const IMessagePtr& out);

            /// @copydoc klk::IModule::registerProcessors
            virtual void registerProcessors();

            /// @copydoc IModule::getType
            virtual mod::Type getType() const throw() {return mod::MOD;}
        private:
            /**
               Copy constructor
            */
            TestModule4ASync(const TestModule4ASync&);

            /**
               Assignment opearator
            */
            TestModule4ASync& operator=(const TestModule4ASync&);
        };

        /// Module smart pointer
        typedef boost::shared_ptr<TestModule4ASync> TestModule4ASyncPtr;

        /**
           @brief Test module for messaging core tests

           Test module for messaging core tests
        */
        class TestModule4Sync : public klk::Module
        {
        public:
            /**
               Constructor
            */
            TestModule4Sync(IFactory *factory);

            /**
               Destructor
            */
            virtual ~TestModule4Sync(){}

            /**
               @return response message
            */
            const IMessagePtr& getResponse(){return m_msg_holder;}
        private:
            IMessagePtr m_msg_holder; ///< message holder

            /**
               Gets a human readable module name

               @return the name
            */
            virtual const std::string getName() const throw()
            {return "msgcoretest4sync";}

            /**
               Process an async message

               @param[in] msg the message to be processed

               @exception @ref klk::ERROR
            */
            void processASync(const IMessagePtr& msg);

            /// @copydoc klk::IModule::registerProcessors
            virtual void registerProcessors();

            /// @copydoc klk::IModule::getType
            virtual mod::Type getType() const throw() {return mod::MOD;}
        private:
            /**
               Copy constructor
            */
            TestModule4Sync(const TestModule4Sync&);

            /**
               Assignment opearator
            */
            TestModule4Sync& operator=(const TestModule4Sync&);
        };

        /// Module smart pointer
        typedef boost::shared_ptr<TestModule4Sync> TestModule4SyncPtr;

        /** @} */
    }
}

#endif //KLK_TESTHELPMODULE_H
