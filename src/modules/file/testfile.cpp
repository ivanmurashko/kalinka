/**
   @file testfile.cpp
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
   - 2009/03/18 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "testfile.h"
#include "defines.h"
#include "testdefines.h"
#include "testid.h"
#include "file.h"
#include "testhelpmodule.h"
#include "testutils.h"
#include "testcli.h"
#include "traps.h"

// modules specific
#include "../checkdb/defines.h"

using namespace klk;
using namespace klk::file;

// The function inits module's unit test
void klk_module_test_init()
{
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestFile,
                                          MODNAME);
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestCLI,
                                          MODNAME);
    CPPUNIT_REGISTRY_ADD(MODNAME, test::ALL);
}

//
// TestFile class
//


// Constructor
TestFile::TestFile() :
    test::TestModuleWithDB("testfile.sql")
{
}

// Loads all necessary modules at setUp()
void TestFile::loadModules()
{
    CPPUNIT_ASSERT(m_modfactory);
    // now load the it
    m_modfactory->load(MODID);

    // add helper module
    this->addDebug<TestHelpModule>();
    m_modfactory->load(TEST_MOD_ID);
}


// The unit test itself
void TestFile::test()
{
    test::printOut("\nFile test ... ");

    testInitial();

    // db checker
    sleep(db::CHECKDBINTERVAL + 2);

    // should not changed
    testInitial();

    // main test
    testMain();
}

// Tests initial values
void TestFile::testInitial()
{
    sleep(2);
    // we should have 3 records at our file list
    CPPUNIT_ASSERT(getModule<File>(MODID)->m_info.size() == 3);
    FileInfoList list = getModule<File>(MODID)->m_info.getInfoList();
    CPPUNIT_ASSERT(list.size() == 3);

    FileInfoList::iterator find1 = std::find_if(
        list.begin(), list.end(),
        boost::bind(mod::FindInfoByUUID<>(), _1, TESTUUID1));
    CPPUNIT_ASSERT(find1 != list.end());
    CPPUNIT_ASSERT((*find1)->getName() == TESTNAME1);
    CPPUNIT_ASSERT((*find1)->getPath() == TESTPATH1);
    CPPUNIT_ASSERT((*find1)->getType() == type::REGULAR);
    FileInfoList::iterator find2 = std::find_if(
        list.begin(), list.end(),
        boost::bind(mod::FindInfoByUUID<>(), _1, TESTUUID2));
    CPPUNIT_ASSERT(find2 != list.end());
    CPPUNIT_ASSERT((*find2)->getName() == TESTNAME2);
    CPPUNIT_ASSERT((*find2)->getPath() == TESTPATH2);
    CPPUNIT_ASSERT((*find2)->getType() == type::REGULAR);
    FileInfoList::iterator find3 = std::find_if(
        list.begin(), list.end(),
        boost::bind(mod::FindInfoByUUID<>(), _1, TESTUUID3));
    CPPUNIT_ASSERT(find3 != list.end());
    CPPUNIT_ASSERT((*find3)->getName() == TESTNAME3);
    CPPUNIT_ASSERT((*find3)->getPath() == TESTPATH3);
    CPPUNIT_ASSERT((*find3)->getType() == type::FOLDER);
}

// Main test
void TestFile::testMain()
{
    // start the processing
    IMessagePtr start_msg = m_msgfactory->getMessage(TEST_MSG_ID);
    CPPUNIT_ASSERT(start_msg);
    m_modfactory->sendMessage(start_msg);

    sleep(4);

    Result rc = getModule<TestHelpModule>(TEST_MOD_ID)->getResult();
    CPPUNIT_ASSERT(rc == OK);

    // test traps
    test::TrapInfoVector result = getTraps();
    CPPUNIT_ASSERT(result.size() == 1);
    CPPUNIT_ASSERT(test::IsOIDMatch()(result[0], TRAP_FILEMISSING) == true);
}

