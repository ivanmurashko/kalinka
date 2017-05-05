/**
   @file testmodules.h
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
   - 2010/09/25 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTMODULES_H
#define KLK_TESTMODULES_H

#include <list>

#include <boost/shared_ptr.hpp>

#include "testlauncher.h"
#include "module.h"
#include "launchmodule.h"

namespace klk
{
    namespace launcher
    {
        /** @defgroup grLauncherNodulesTest
            @brief Modules unit test

            There is a test for working with modules in launcher application

            @ingroup grLauncherTest

            @{
        */

        /**
           @brief Base class for helper modules

           The class keeps several common utilities used by
           both remote and main module
        */
        class TestBase
        {
        public:
            /// Constructor
            TestBase();

            /// Destructor
            virtual ~TestBase(){}

            /**
               Check is the module got a sync message from main module

               @param[in] id - the message id to be checked

               @return
               - true the module got a sync message
               - false the module don't get it
            */
            bool checkSync(const std::string& id)
            {
                return (std::find(m_get_sync.begin(), m_get_sync.end(), id) != m_get_sync.end());
            }

            /**
               Check is the module got an async message from main module

               @param[in] id - the message id to be checked

               @return
               - true the module got an async message
               - false the module don't get it
            */
            bool checkASync(const std::string& id)
            {
                return (std::find(m_get_async.begin(), m_get_async.end(), id) !=
                        m_get_async.end());
            }

            /**
               Do processing of a sync message

               @param[in] in - the input message
               @param[out] out - the output message

               @exception @ref klk::Exception
            */
            void processSync(const IMessagePtr& in, const IMessagePtr& out);

            /**
               Do processing of an async message

               @param[in] in - the input message

               @exception @ref klk::Exception
            */
            void processASync(const IMessagePtr& in);
        private:
            /// typedef
            typedef std::list<std::string> CatchResultList;
            CatchResultList m_get_sync; ///< the module got a sync message flag
            CatchResultList m_get_async; ///< the module got an async message flag
        private:
            /// Fake copy constructor
            TestBase(const TestBase&);

            /// Fake assigment operator
            TestBase& operator=(const TestBase&);
        };

        /**
           @brief Helper module

           Unit test checks the module loading.
           The module should be loaded remotelly
           (inside main  mediaserver application)
        */
        class TestHelpModule : public klk::Module, public TestBase
        {
        public:
            /// Constructor
            TestHelpModule(IFactory* factory);

            /// Destructor
            virtual ~TestHelpModule(){}
        private:
            /// @copydoc klk::IModule::registerProcessors
            virtual void registerProcessors();

            /// @copydoc klk::IModule::getType
            virtual mod::Type getType() const throw() {return mod::MOD;}

            /// @copydoc klk::IModule::getName
            virtual const std::string getName() const throw()
            {return "launcher/modulestest/helper";}
        private:
            /// Fake copy constructor
            TestHelpModule(const TestHelpModule&);

            /// Fake assigment operator
            TestHelpModule& operator=(const TestHelpModule&);
        };

        /// Main module smart pointer
        typedef boost::shared_ptr<TestHelpModule> TestHelpModulePtr;

        /**
           @brief The main application module

           The main application module to be used in modules test.

           It should be loaded inside klklauncher application
        */
        class TestMainModule : public launcher::Module, public TestBase
        {
        public:
            /// Constructor
            TestMainModule();

            /// Destructor
            virtual ~TestMainModule(){}
        private:
            /// @copydoc klk::IModule::getName
            virtual const std::string getName() const throw()
            {return "launcher/modulestest/main";}

            /// @copydoc klk::ModuleWithDB::processDB
            virtual void processDB(const IMessagePtr& msg)
            {
            }

            /// @copydoc klk::IModule::registerProcessors
            virtual void registerProcessors();
        private:
            /// Fake copy constructor
            TestMainModule(const TestMainModule&);

            /// Fake assigment operator
            TestMainModule& operator=(const TestMainModule&);
        };

        /// Main module smart pointer
        typedef boost::shared_ptr<TestMainModule> TestMainModulePtr;

        /**
           @brief Modules test

           The unit test checks working with modules via RPC
           Only main module has to be loaded explicitly all others
           have to be loaded remotely
        */
        class TestModules : public TestLauncher
        {
            CPPUNIT_TEST_SUITE(TestModules);
            CPPUNIT_TEST(test);
            CPPUNIT_TEST_SUITE_END();
        public:
            /// Constructor
            TestModules();

            /// Destructor
            virtual ~TestModules();

            /// The test itself
            void test();

            /// @copydoc klk::launcher::TestLauncher::tearDown
            virtual void tearDown();
        private:
            TestHelpModulePtr m_help_module; ///< helper module for tests
            TestMainModulePtr m_main_module; ///< main module for tests

            /// @copydoc klk::test::TestModule::loadModules()
            virtual void loadModules();

            /// test the module load/unload via RPC
            void testLoad();

            /// test the messages sending via RPC
            void testMessages();
        private:
            /// Fake copy constructor
            TestModules(const TestModules&);

            /// Fake assigment operator
            TestModules& operator=(const TestModules&);
        };

        /** @} */
    }
}

#endif //KLK_TESTMODULES_H
