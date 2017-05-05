/**
   @file teststreamer.cpp
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
   - 2008/10/12 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cppunit/extensions/HelperMacros.h>

#include "teststreamer.h"
#include "log.h"
#include "defines.h"
#include "streamer.h"
#include "testid.h"
#include "testhttpthread.h"
#include "testdefines.h"

// tests
#include "testcli.h"
#include "teststartup.h"
#include "testtcp.h"
#include "testudp.h"
#include "testsnmp.h"
#include "testtheora.h"
#include "testslowconnection.h"


// modules specific info
#include "service/messages.h"
#include "network/defines.h"

using namespace klk;
using namespace klk::http;

// The function inits module's unit test
void klk_module_test_init()
{
    const std::string TESTSLOW = MODNAME + "/slow";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestSlowConnection, TESTSLOW);
    CPPUNIT_REGISTRY_ADD(TESTSLOW, MODNAME);

    const std::string TESTUDP = MODNAME + "/udp";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestUDP, TESTUDP);
    CPPUNIT_REGISTRY_ADD(TESTUDP, MODNAME);

    const std::string TESTTCP = MODNAME + "/tcp";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestTCP, TESTTCP);
    CPPUNIT_REGISTRY_ADD(TESTTCP, MODNAME);

    const std::string TESTSTARTUP = MODNAME + "/startup";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestStartup, TESTSTARTUP);
    CPPUNIT_REGISTRY_ADD(TESTSTARTUP, MODNAME);

    const std::string TESTCLI = MODNAME + "/cli";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestCLI, TESTCLI);
    CPPUNIT_REGISTRY_ADD(TESTCLI, MODNAME);

    const std::string TESTSNMP = MODNAME + "/snmp";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestSNMP, TESTSNMP);
    CPPUNIT_REGISTRY_ADD(TESTSNMP, MODNAME);

    const std::string TESTTHEORA = MODNAME + "/theora";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestTheora, TESTTHEORA);
    CPPUNIT_REGISTRY_ADD(TESTTHEORA, MODNAME);

    CPPUNIT_REGISTRY_ADD(MODNAME, test::ALL);
}


//
// TestStreamer class
//

// There are a lot of such errors on darwin
//  Error 24 in pipe(): Too many open files
#ifdef DARWIN
const u_int THREADNUM = 4;
#else
const u_int THREADNUM = 10;
#endif

// Constructor
TestStreamer::TestStreamer(const std::string& sqlscript) :
    test::TestModuleWithDB(sqlscript),
    m_scheduler(), m_consumers()
{
}

// Test constructor
void TestStreamer::setUp()
{
    for (u_int i = 0; i < THREADNUM; i++)
    {
        test::ThreadPtr thread(new TestConsumerThread());
        m_scheduler.addTestThread(thread);
        m_consumers.push_back(thread);
    }

    test::TestModuleWithDB::setUp();

    sleep(3);
}

// Destructor
void TestStreamer::tearDown()
{
    test::TestModuleWithDB::tearDown();
    // clear all
    m_scheduler.stop();
    m_scheduler.clear();
    m_consumers.clear();
}

// Loads all necessary modules at setUp()
void TestStreamer::loadModules()
{
    CPPUNIT_ASSERT(m_modfactory);

    // load adapter first
    // this is necessary because the main module (streamer)
    // is derived from klk::launcher::app::Module that
    // requires adapter to be run in constructor
    loadAdapter();

    // now load the streamer
    m_modfactory->load(MODID);
    // wait for awhile
    sleep(2);
}

