/**
   @file testcheckdb.cpp
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
   - 2009/03/19 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testcheckdb.h"
#include "testid.h"
#include "testdefines.h"
#include "defines.h"
#include "exception.h"
#include "testhelpmodule.h"
#include "db.h"
#include "testutils.h"

using namespace klk;
using namespace klk::db;

// The function inits module's unit test
void klk_module_test_init()
{
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestCheckDB,
                                          MODNAME);
    CPPUNIT_REGISTRY_ADD(MODNAME, test::ALL);
}

//
// TestCheckDB class
//

// Constructor
TestCheckDB::TestCheckDB() :
    test::TestModuleWithDB("testcheckdb.sql")
{
}

// Loads modules
void TestCheckDB::loadModules()
{
    // add help module
    addDebug<db::TestModule>();
    // load the help module
    m_modfactory->load(TESTMODID);
}

// The async. messages test
void TestCheckDB::test()
{
    test::printOut("\nCheck DB test ... ");

    // wait for awail
    sleep(2);

    // the main module should be loaded
    // automatically
    CPPUNIT_ASSERT(m_modfactory->isLoaded(MODID) == true);

    boost::shared_ptr<db::TestModule>
        testmod = getModule<db::TestModule>(TESTMODID);

    // test that initially we are clearing
    // NOTE: just after startup we send update db notification to
    // any module that depends on the db. Thus initial count value is 1
    CPPUNIT_ASSERT(testmod->getCount() == 1);
    CPPUNIT_ASSERT(m_db);


    // update db 1 st time
    Parameters params;
    params.add("@return_value");
    Result res1 = m_db->callSimple("test_checkdb_add", params);
    CPPUNIT_ASSERT(res1["@return_value"].toInt() == 0);
    sleep(CHECKDBINTERVAL + 2);

    // test that we have 1 updates
    CPPUNIT_ASSERT(testmod->getCount() == 2);

    // update db 2d time
    // we do 2 updates but should get only one db change event
    Result res2 = m_db->callSimple("test_checkdb_add", params);
    CPPUNIT_ASSERT(res2["@return_value"].toInt() == 0);
    Result res3 = m_db->callSimple("test_checkdb_add", params);
    CPPUNIT_ASSERT(res3["@return_value"].toInt() == 0);
    sleep(CHECKDBINTERVAL + 2);


    // test that we have 2 updates
    CPPUNIT_ASSERT(testmod->getCount() == 3);
}

