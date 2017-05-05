/**
   @file testlauncher.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testlauncher.h"
#include "testresources.h"
#include "testmodules.h"

#include "launchfactory.h"
#include "paths.h"

#include "testid.h"
#include "testdefines.h"

using namespace klk;
using namespace klk::launcher;

// The function inits module's unit test
void klk_module_test_init()
{
    const std::string TESTRESOURCES = APPNAME + "/resources";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestResources,
                                          TESTRESOURCES);
    CPPUNIT_REGISTRY_ADD(TESTRESOURCES, APPNAME);
    const std::string TESTMODULES = APPNAME + "/modules";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestModules,
                                          TESTMODULES);
    CPPUNIT_REGISTRY_ADD(TESTMODULES, APPNAME);

    CPPUNIT_REGISTRY_ADD(APPNAME, test::ALL);
}

//
// TestLauncher class
//

/// Constructor
TestLauncher::TestLauncher() :test::TestModuleWithDB("testlauncher.sql")
{
}

/// Destructor
TestLauncher::~TestLauncher()
{
}

/// @copydoc klk::test::TestModule::setUp()
void TestLauncher::setUp()
{
    // launcher factory initialization
    // adn default config setup (to use test config instead of main)
    Factory::instance("testlauncher", MAINMODID) ->getConfig()->setPath(dir::CFG_TEST);
    // load the config
    Factory::instance()->getConfig()->load();
    // the parent class initialization
    test::TestModuleWithDB::setUp();
}

/// @copydoc klk::test::TestModule::tearDown()
void TestLauncher::tearDown()
{
    // unload all modules
    Factory::instance()->getModuleFactory()->unloadAll();
    test::TestModule::tearDown();
    // destroy the launcher factory
    Factory::destroy();
}


/// @copydoc klk::test::TestModule::loadModules()
void TestLauncher::loadModules()
{
    // the adapter module is necessary
    loadAdapter();
}
