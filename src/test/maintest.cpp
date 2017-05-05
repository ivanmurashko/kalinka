/**
   @file maintest.cpp
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
   - 2009/04/04 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/assert.hpp>
#include <boost/bind.hpp>

#include "utils.h"
#include "exception.h"
#include "maintest.h"
#include "testutils.h"
#include "testfactory.h"

#include "../modules/service/defines.h"
#include "../modules/adapter/defines.h"
#include "../modules/msgcore/defines.h"

using namespace klk::test;

const std::string TESTFOLDER("/tmp/test");

//
// Main class
//

// Constructor
void Main::setUp()
{
    base::Utils::unlink(TESTFOLDER);
}

// Destructor
void Main::tearDown()
{
    base::Utils::unlink(TESTFOLDER);
}

// Tests utils
void Main::testUtils()
{
    printOut("\nMain utilities test ...");
    // test folders creation/delete
    const std::string parent = TESTFOLDER + ("/parent");
    const std::string child = parent + "/child";

    CPPUNIT_ASSERT(base::Utils::fileExist(child) == false);
    CPPUNIT_ASSERT(base::Utils::fileExist(parent) == false);
    CPPUNIT_ASSERT(base::Utils::isDir(child) == false);
    CPPUNIT_ASSERT(base::Utils::isDir(parent) == false);

    base::Utils::mkdir(child);

    CPPUNIT_ASSERT(base::Utils::fileExist(child) == true);
    CPPUNIT_ASSERT(base::Utils::fileExist(parent) == true);
    CPPUNIT_ASSERT(base::Utils::isDir(child) == true);
    CPPUNIT_ASSERT(base::Utils::isDir(parent) == true);
}

// functor to find a module by id
struct FindModuleByID
{
    /// the functor itself
    bool operator() (const klk::IModulePtr module, const std::string& id){
        BOOST_ASSERT(module);
        return (module->getID() == id);
    }
};


// The modules test
// It tests that all modules can be loaded
void Main::testModules()
{
    printOut("\nModules test ...");

    // tests that all modules can be loaded
    IModuleFactory* factory = test::Factory::instance()->getModuleFactory();
    // will produce exception in the case of error
    printOut("\n\tklk::IModuleFactory::getModules() ...");
    ModuleList list1 = factory->getModules();
    testModulesList(list1);
    printOut("\n\tklk::IModuleFactory::getModulesWithoutRegistration() ...");
    ModuleList list2 = factory->getModulesWithoutRegistration();
    testModulesList(list2);
}

// Tests that BOOST_ASSERT throws a klk::Exception
void Main::testBoostAssert()
{
    printOut("\nBOOST_ASSERT test ...");
    BOOST_ASSERT(false);
}

// tests list of modules passed as the argument
void Main::testModulesList(const ModuleList& list)
{
    BOOST_ASSERT(!list.empty());

    // the test selfcheck
    BOOST_ASSERT(
        std::find_if(
            list.begin(), list.end(),
            boost::bind<bool>(FindModuleByID(), _1, "00000")) == list.end()
        );


    // checks that base modules are in the list
    BOOST_ASSERT(
        std::find_if(
            list.begin(), list.end(),
            boost::bind<bool>(FindModuleByID(),
                              _1, klk::msgcore::MODID)) != list.end()
        );
    BOOST_ASSERT(
        std::find_if(
            list.begin(), list.end(),
            boost::bind<bool>(FindModuleByID(),
                              _1, klk::adapter::MODID)) != list.end()
        );
    BOOST_ASSERT(
        std::find_if(
            list.begin(), list.end(),
            boost::bind<bool>(FindModuleByID(),
                              _1, klk::srv::MODID)) != list.end()
        );
}

