/**
   @file testdepstart.h
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
   - 2010/12/09 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTDEPSTART_H
#define KLK_TESTDEPSTART_H

#include "testmodule.h"
#include "moduledb.h"

namespace klk
{
    namespace msgcore
    {
        /** @defgroup grTestDepMsgCore Dependency unit test

            The test check modules startup if one of the modules
            depends on another one. The second module should wait until
            the first module complete its startup and will be able to process
            incomming messages

            @note see ticket #238 for more info

            @ingroup grTestMsgCore

            @{
        */

        /**
           @brief Helper module class

           Base helper module class for dependency test
        */
        class DepHelpModuleBase : public ModuleWithDB
        {
        public:
            /// Constructor
            DepHelpModuleBase(IFactory* factory, const std::string& modid);

            /// Destructor
            virtual ~DepHelpModuleBase(){}

            /**
               @copydoc klk::ModuleWithDB::processDB

               @note stores the processing start time for future tests
            */
            virtual void processDB(const IMessagePtr& msg)
            {
                m_start_time = time(NULL);
            }

            /// @return the start processing time
            time_t getStartTime() const
            {
                return m_start_time;
            }
        private:
            time_t m_start_time; ///< the processing start time

            /// @copydoc klk::IModule::getType
            virtual mod::Type getType() const throw() {return mod::MOD;}
        private:
            /// Fake copy constructor
            DepHelpModuleBase(const DepHelpModuleBase&);

            /// Fake assigment operator
            DepHelpModuleBase& operator=(const DepHelpModuleBase&);
        };

        /**
           @brief Parent module

           The module is the parent that do a delay 5 seconds on startup
        */
        class DepHelpModuleParent : public DepHelpModuleBase
        {
        public:
            /// Constructor
            DepHelpModuleParent(IFactory* factory);

            /// Destructor
            virtual ~DepHelpModuleParent(){}
        private:
            /**
               @copydoc klk::Module::preMainLoop

               @note do a delay 5 seconds on startup
             */
            virtual void preMainLoop()
            {
                sleep(5);
                DepHelpModuleBase::preMainLoop();
            }

            /// @copydoc klk::IModule::getName
            virtual const std::string getName() const throw()
            {return "parent";}
        private:
            /// Fake copy constructor
            DepHelpModuleParent(const DepHelpModuleParent&);

            /// Fake assigment operator
            DepHelpModuleParent& operator=(const DepHelpModuleParent&);
        };

        /**
           @brief Child dependency module

           The module depends on DepHelpModuleParent
           and should start the processing DB request after it
        */
        class DepHelpModuleChild : public DepHelpModuleBase
        {
        public:
            /// Constructor
            DepHelpModuleChild(IFactory* factory);

            /// Destructor
            virtual ~DepHelpModuleChild(){}
        private:
            /// @copydoc klk::IModule::getName
            virtual const std::string getName() const throw()
            {return "child";}
        private:
            /// Fake copy constructor
            DepHelpModuleChild(const DepHelpModuleChild&);

            /// Fake assigment operator
            DepHelpModuleChild& operator=(const DepHelpModuleChild&);
        };

        /**
           @brief Dependency test

           The dependency test
        */
        class TestDependency : public test::TestModule
        {
            CPPUNIT_TEST_SUITE(TestDependency);
            CPPUNIT_TEST(test);
            CPPUNIT_TEST_SUITE_END();
        public:
            /// Constructor
            TestDependency();

            /// Destructor
            virtual ~TestDependency(){}

            /// Test itself
            void test();
        private:
            /// The helper module smart pointer
            typedef boost::shared_ptr<DepHelpModuleBase> DepHelpModuleBasePtr;

            DepHelpModuleBasePtr m_parent; ///< parent module
            DepHelpModuleBasePtr m_child; ///< child module

            /// Loads all necessary modules at setUp()
            virtual void loadModules();
        };

        /** @} */
    }
}

#endif //KLK_TESTDEPSTART_H
