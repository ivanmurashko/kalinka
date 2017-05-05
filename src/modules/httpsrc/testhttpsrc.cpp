/**
   @file testhttpsrc.cpp
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
   - 2009/11/21 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testhttpsrc.h"
#include "defines.h"
#include "testid.h"
#include "httpsrc.h"
#include "testutils.h"
#include "testcli.h"
#include "testhelpmodule.h"


using namespace klk;
using namespace klk::httpsrc;

// The function inits module's unit test
void klk_module_test_init()
{
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestHTTPSrc,
                                          MODNAME);
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestCLI,
                                          MODNAME);
    CPPUNIT_REGISTRY_ADD(MODNAME, test::ALL);
}

//
// TestHTTPSrc class
//


// Constructor
TestHTTPSrc::TestHTTPSrc() :
    test::TestModuleWithDB("testhttpsrc.sql")
{
}

// Loads all necessary modules at setUp()
void TestHTTPSrc::loadModules()
{
    CPPUNIT_ASSERT(m_modfactory);
    // now load the it
    m_modfactory->load(MODID);

    // add helper module
    this->addDebug<TestHelpModule>();
    // load helper module
    m_modfactory->load(test::HELP_MOD_ID);
}


// The unit test itself
void TestHTTPSrc::test()
{
    test::printOut("\nHTTP source test ... ");

    // give some time to module load
    sleep(3);

    // start the processing
    IMessagePtr start_msg = m_msgfactory->getMessage(test::HELP_MSG_ID);
    CPPUNIT_ASSERT(start_msg);
    m_modfactory->sendMessage(start_msg);

    sleep(10);

    Result rc = getModule<TestHelpModule>(test::HELP_MOD_ID)->getResult();
    CPPUNIT_ASSERT(rc == OK);
}

