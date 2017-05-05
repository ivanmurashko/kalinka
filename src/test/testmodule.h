/**
   @file testmodule.h
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
   - 2008/08/23 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLKTEST_TESTMODULE_H
#define KLKTEST_TESTMODULE_H

#include <cppunit/extensions/HelperMacros.h>

#include <string>

#include "dbscriptlauncher.h"
#include "snmp/trapreceiver.h"
#include "testfactory.h"
#include "testmodfactory.h"
#include "db.h"
#include "exception.h"

namespace klk
{
    namespace test
    {
        /**
           @brief Base class for module unit test

           Base class for module unit test

           @ingroup grTest
        */
        class TestModule : public CppUnit::TestFixture
        {
        public:
            /**
               Constructor
            */
            TestModule();

            /**
               Destructor
            */
            virtual ~TestModule();

            /**
               @brief Prepares the unit test data

               Fills initial data for utest
            */
            virtual void setUp();

            /**
               @brief Clears utest data

               Clears utest data
            */
            virtual void tearDown();
        protected:
            klk::test::ModuleFactory* m_modfactory; ///< module factory
            IMessageFactory* m_msgfactory; ///< message factory

            /**
               @brief Loads necessary modules

               Loads all necessary modules at setUp()
            */
            virtual void loadModules() = 0;

            /**
               Gets the received traps
            */
            const TrapInfoVector getTraps() const
            {
                return m_snmp_receiver.getTraps();
            }

            /**
               Loads adapter module
            */
            void loadAdapter();

            /**
               Retrives the real module class by it's id

               @param[in] modid - the module id

               @return the module
            */
            template <class T> boost::shared_ptr<T>
                getModule(const std::string& modid)
            {
                IFactory * factory = test::Factory::instance();
                CPPUNIT_ASSERT(factory);
                IModulePtr
                    module = factory->getModuleFactory()->getModule(modid);
                CPPUNIT_ASSERT(module);
                return boost::dynamic_pointer_cast<T, IModule>(module);
            }

            template <class T> void addDebug()
            {
                IFactory * factory = test::Factory::instance();
                CPPUNIT_ASSERT(factory);
                CPPUNIT_ASSERT(m_modfactory);
                m_modfactory->addDebug(IModulePtr(new T(factory)));
            }
        private:
            SNMPReceiver m_snmp_receiver; ///< SNMP receiver
        private:
            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestModule(const TestModule& value);


            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestModule& operator=(const TestModule& value);
        };

        /**
           @brief Base class for module unit test

           Base class for module unit test

           @ingroup grTest
        */
        class TestModuleWithDB : public TestModule
        {
        public:
            /**
               Constructor

               @param[in] sqlscript - the sql script to be executed
            */
            explicit TestModuleWithDB(const std::string& sqlscript);

            /**
               Constructor
            */
            TestModuleWithDB();

            /**
               Destructor
            */
            virtual ~TestModuleWithDB();

            /**
               Test constructor
            */
            virtual void setUp();

            /**
               Destructor
            */
            virtual void tearDown();
        protected:
            DBScriptLauncher* m_launcher; ///< db script launcher
            db::DB *m_db; ///< db connection
        private:
            std::string m_sqlscript; ///< data creation script

            /**
               Copy constructor
               @param[in] value - the copy param
            */
            TestModuleWithDB(const TestModuleWithDB& value);


            /**
               Assigment operator
               @param[in] value - the copy param
            */
            TestModuleWithDB& operator=(const TestModuleWithDB& value);
        };
    }
}

#endif //KLKTEST_TESTMODULE_H
