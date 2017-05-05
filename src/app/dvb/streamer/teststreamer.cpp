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

#include <time.h>

#include <algorithm>

#include "teststreamer.h"
#include "streamer.h"
#include "log.h"
#include "testid.h"
#include "defines.h"
#include "streamer.h"
#include "testfactory.h"
#include "resources.h"
#include "testcli.h"
#include "db.h"
#include "testdefines.h"
#include "testplugin.h"
#include "streamerutils.h"
#include "testsnmp.h"
#include "testutils.h"

// modules specific info
#include "dvb/dvbdev.h"
#include "dvb/messages.h"
#include "dvb/defines.h"
#include "checkdb/defines.h"

using namespace klk;
using namespace klk::dvb::stream;

// The function inits module's unit test
void klk_module_test_init()
{
    const std::string TESTMAIN = MODNAME + "/main";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestStreamer, TESTMAIN);
    CPPUNIT_REGISTRY_ADD(TESTMAIN, MODNAME);

    const std::string TESTCLI = MODNAME + "/cli";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestCLI, TESTCLI);
    CPPUNIT_REGISTRY_ADD(TESTCLI, MODNAME);

    const std::string TESTPLUGIN = MODNAME + "/plugin";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestPlugin, TESTPLUGIN);
    CPPUNIT_REGISTRY_ADD(TESTPLUGIN, MODNAME);

    const std::string TESTSNMP = MODNAME + "/snmp";
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestSNMP, TESTSNMP);
    CPPUNIT_REGISTRY_ADD(TESTSNMP, MODNAME);

    CPPUNIT_REGISTRY_ADD(MODNAME, test::ALL);
}

//
// TestStreamer class
//


// Constructor
TestStreamer::TestStreamer() : TestBase()
{
}

// The test checks failures stations processing
void TestStreamer::testFailures()
{
    test::printOut("\nDVB Streamer failure test ... ");

    test::printOut("\n\tPreparation ... ");
    // add two channels
    testFirst();
    // test failures base
    testFailuresBase();
    // do base test now
    testChannels();
}

// The unit test itself
void TestStreamer::testMain()
{
    test::printOut("\nDVB Streamer test ... ");

    test::printOut("\n\tInitial test ... ");
    // test before (no streaming channels)
    sleep(db::CHECKDBINTERVAL + 5);
    CPPUNIT_ASSERT(getModule<Streamer>(MODID)->m_info.empty() == true);
    testChannels();

    test::printOut("\n\t1 st streaming test ... ");
    testFirst();

    test::printOut("\n\tAdd/remove test ... ");
    testAddRemove();

    test::printOut("\n\tRestart ... ");
    testRestart();

    test::printOut("\n\tProblems test ... ");
    testProblems();
}

// Test channels
void TestStreamer::testChannels()
{
    if (m_thrfactory->empty())
    {
        // empty
        CPPUNIT_ASSERT(getModule<Streamer>(MODID)->m_info.empty());
        return;
    }

    // not empty
    CPPUNIT_ASSERT(getModule<Streamer>(MODID)->m_info.empty() == false);

    Streamer::InfoList list =
        getModule<Streamer>(MODID)->m_info.getInfoList();

    // check by names
    for (Streamer::InfoList::iterator i = list.begin();
         i != list.end(); i++)
    {
        const std::string name = (*i)->getName();

        CPPUNIT_ASSERT(m_thrfactory->isStream(name));
    }
}

// First test after startup
void TestStreamer::testFirst()
{
    m_dev1->setParam(dev::HASLOCK, "0");
    // start the processing
    // add two channels
    Utils(test::Factory::instance()).addStation(TESTSTATION1, TESTROUTE1);
    Utils(test::Factory::instance()).addStation(TESTSTATION3, TESTROUTE3);
    Utils(test::Factory::instance()).addStation(TESTSTATION2, TESTROUTE2);

    sleep(db::CHECKDBINTERVAL + 5);

    // test that we have several channels
    // there should not be empty list
    // it should include currently streamed channels
    CPPUNIT_ASSERT(getModule<Streamer>(MODID)->m_info.empty() == false);
    Streamer::InfoSet testset = getModule<Streamer>(MODID)->getInfoFromDB();
    CPPUNIT_ASSERT(testset.empty() == false);
    CPPUNIT_ASSERT(testset.size() == 2);
    CPPUNIT_ASSERT(getModule<Streamer>(MODID)->m_info.size() == 2);

    CPPUNIT_ASSERT(m_thrfactory->isStream(TESTSTATION1));
    CPPUNIT_ASSERT(m_thrfactory->isStream(TESTSTATION2));
    CPPUNIT_ASSERT(m_thrfactory->isStream(TESTSTATION3) == false);

    // do base test now
    testChannels();
}

// Test add/remove channels
void TestStreamer::testAddRemove()
{
    m_dev1->setParam(dev::HASLOCK, "1");
    // del a channel
    Utils(test::Factory::instance()).delStation(TESTSTATION1);
    // one channel left
    sleep(db::CHECKDBINTERVAL + 5);
    // test that we have one channel
    CPPUNIT_ASSERT(getModule<Streamer>(MODID)->m_info.empty() == false);
    CPPUNIT_ASSERT(getModule<Streamer>(MODID)->m_info.size() == 1);
    CPPUNIT_ASSERT(m_thrfactory->isStream(TESTSTATION1) == false);
    CPPUNIT_ASSERT(m_thrfactory->isStream(TESTSTATION2) == true);
    CPPUNIT_ASSERT(m_thrfactory->isStream(TESTSTATION3) == false);
    testChannels();

    m_dev1->setParam(dev::HASLOCK, "1");
    // add the removed
    Utils(test::Factory::instance()).addStation(TESTSTATION1, TESTROUTE1);
    sleep(db::CHECKDBINTERVAL + 5);
    // test that we have 2 channel
    CPPUNIT_ASSERT(getModule<Streamer>(MODID)->m_info.empty() == false);
    CPPUNIT_ASSERT(getModule<Streamer>(MODID)->m_info.size() == 2);
    CPPUNIT_ASSERT(m_thrfactory->isStream(TESTSTATION1) == true);
    CPPUNIT_ASSERT(m_thrfactory->isStream(TESTSTATION2) == true);
    CPPUNIT_ASSERT(m_thrfactory->isStream(TESTSTATION3) == false);
    testChannels();
}

// Test restart
void TestStreamer::testRestart()
{
    // restart
    m_modfactory->unload(MODID);
    sleep(3);
    // just update dev state to make dvb module happy
    m_dev1->setParam(dev::HASLOCK, "1");
    m_modfactory->load(MODID);
    // one channel left
    sleep(2*db::CHECKDBINTERVAL + 5);
    // test that we have 2 channels (after prev. scan)
    CPPUNIT_ASSERT(getModule<Streamer>(MODID)->m_info.empty() == false);
    CPPUNIT_ASSERT(getModule<Streamer>(MODID)->m_info.size() == 2);
    CPPUNIT_ASSERT(m_thrfactory->isStream(TESTSTATION1));
    CPPUNIT_ASSERT(m_thrfactory->isStream(TESTSTATION2));
    testChannels();
}

// Base test for failures
void TestStreamer::testFailuresBase()
{
    // there will be an additional reread of the db
    sleep(db::CHECKDBINTERVAL + 5);
    time_t start = time(NULL);
    test::printOut("\n\tTest failures exclusion ... ");
    // now we will wait
    // and check that there is no any DB processing during the
    // time period
    // test before (no streaming channels)
    sleep(2 * db::CHECKDBINTERVAL + 5);
    time_t last_dbupdate =
        getModule<Streamer>(MODID)->m_last_dbupdate.getValue();
    CPPUNIT_ASSERT(last_dbupdate < start);
    test::printOut("\n\tTest failures restore ... ");
    // now we init second device with a correct source
    m_dev2->setParam(dev::SOURCE, m_dev1->getStringParam(dev::SOURCE));
    // wait for awhile
    sleep(2 * FAILURES_UPDATEINTERVAL + 5);
    // check that we have all devs enabled
    last_dbupdate = getModule<Streamer>(MODID)->m_last_dbupdate.getValue();
    CPPUNIT_ASSERT(last_dbupdate > start); // we had an additional db read

    // test that we have several channels
    // there should not be empty list
    // it should include currently streamed channels
    CPPUNIT_ASSERT(getModule<Streamer>(MODID)->m_info.empty() == false);
    CPPUNIT_ASSERT(getModule<Streamer>(MODID)->m_info.size() == 3);

    CPPUNIT_ASSERT(m_thrfactory->isStream(TESTSTATION1));
    CPPUNIT_ASSERT(m_thrfactory->isStream(TESTSTATION2));
    CPPUNIT_ASSERT(m_thrfactory->isStream(TESTSTATION3));
}

// Promblems tests
void TestStreamer::testProblems()
{
    // send a message about problem with adapter
    // FIXME!!! add additional test
    // we have to remove station3 to prevet it to be streamed and locked the
    // card
    Utils(test::Factory::instance()).delStation(TESTSTATION3);
    sleep(db::CHECKDBINTERVAL + 5);
    // test that we have 2 channels (after prev. tests)
    CPPUNIT_ASSERT(getModule<Streamer>(MODID)->m_info.empty() == false);
    CPPUNIT_ASSERT(getModule<Streamer>(MODID)->m_info.size() == 2);
    // we have to get signal lost within dvb::CHECKINTERVAL
    bool was_lost = false;
    time_t end_time = time(NULL) +
        2*(dvb::CHECKINTERVAL + db::CHECKDBINTERVAL + 5);
    while (end_time > time(NULL))
    {
        sleep(1);
        if (getModule<Streamer>(MODID)->m_info.empty() && !was_lost)
        {
            was_lost = true;
            CPPUNIT_ASSERT(m_thrfactory->isStream(TESTSTATION1) == false);
            CPPUNIT_ASSERT(m_thrfactory->isStream(TESTSTATION2) == false);
            end_time = time(NULL) + dvb::CHECKINTERVAL +
                2 * FAILURES_UPDATEINTERVAL + 5;
        }
    }
    CPPUNIT_ASSERT(was_lost);
    // and we now should have the channels
    CPPUNIT_ASSERT(getModule<Streamer>(MODID)->m_info.empty() == false);
    CPPUNIT_ASSERT(getModule<Streamer>(MODID)->m_info.size() == 2);
    testChannels();

    CPPUNIT_ASSERT(m_thrfactory->isStream(TESTSTATION1));
    CPPUNIT_ASSERT(m_thrfactory->isStream(TESTSTATION2));
}
